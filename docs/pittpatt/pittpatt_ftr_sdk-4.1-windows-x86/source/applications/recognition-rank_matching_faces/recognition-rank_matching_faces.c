/**********************************************************************
 
  Face Tracking and Recognition Software Development Kit (FTR-SDK)
  (C) Copyright 2004-2010 Pittsburgh Pattern Recognition

  This software is covered in part by the following patents:

     US Patent 6,829,384
     US Patent 7,194,114
     Pending US Patent Applications

  Portions of this product are manufactured under license from Carnegie
  Mellon University.

**********************************************************************/


/*
 * EXAMPLE PROGRAM : Rank Matching Faces
 *
 * This example reads a list of images of single frontal faces and compares all of
 * the images to each other.  For each image it ranks all of the other images
 * according to how well they match the image in question.
 *
 * If more than one frontal face is found in an image, the SDK determines
 * which face is the most suitable face for recognition.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pittpatt_ftr_sdk.h"
#include "pittpatt_license.h"

#include "pittpatt_raw_image_io.h"

#include "parse_command_line.h"
#include "sdk_utils.h"
#include "sdk_profile_timers.h"

/*****************************************************************************
 * Constants
 *****************************************************************************/

#define FRONTAL_FACES_MASK (PPR_FRONTAL_DETECTOR_MASK | PPR_FRONTAL_TILTED_N30_DETECTOR_MASK | PPR_FRONTAL_TILTED_P30_DETECTOR_MASK)

/*****************************************************************************
 * Functions
 *****************************************************************************/

int main(int argc, char *argv[])
{
  int i;                                                 /* Loop index                                         */
  int j;                                                 /* Loop index                                         */

  parameters_struct params;                              /* Parameters from command-line                       */
  int i_first_file;                                      /* Index of first file name on command line           */
  sdk_utils_file_list_type image_list;                   /* List of images to be ranked                        */
  char *input_file_name;                                 /* Name of input file list                            */
  int n_images;                                          /* Number of images to be processed                   */
  int image_num;                                         /* Index for looping over images                      */

  ppr_context_type context;                              /* SDK detection context                              */

  ppr_raw_image_type raw_image;                          /* Raw image                                          */
  ppr_image_type image;                                  /* Image to be submitted to recognizer                */
  ppr_object_list_type best_frontal_object;              /* Best frontal object found in image                 */

  ppr_object_suitability_type suitability;               /* Value indicating if template can be extracted      */
  ppr_template_type curr_template;                       /* Template for current object in current image       */
  int id;                                                /* ID of current template                             */
  ppr_template_id_list_type template_id_list;            /* ID assigned to each object                         */
  ppr_gallery_type gallery;                              /* Gallery containing template for each image         */
  ppr_similarity_matrix_type similarity_matrix;          /* Similarity matrix between gallery and itself       */
  ppr_similarity_matrix_type adjusted_similarity_matrix; /* Similarity matrix with transitive matching applied */
  char name[PPR_MAX_TEMPLATE_STRING_SIZE];               /* Image name stored in template                      */
  ppr_template_id_list_type ranked_id_list;              /* Ranked template IDs matching image template        */
  ppr_score_list_type score_list;                        /* Score for each ID in ranked list                   */

  /****************************/
  /* Configure and Initialize */
  /****************************/

  sdk_profile_timers_init();
  sdk_profile_timers_name_timer(0, "total");
  sdk_profile_timers_name_timer(1, "initialization");
  sdk_profile_timers_name_timer(2, "read image");
  sdk_profile_timers_name_timer(3, "detect best object");
  sdk_profile_timers_name_timer(4, "extract template");
  sdk_profile_timers_name_timer(5, "self similarity matrix");
  sdk_profile_timers_name_timer(6, "transitive matching");
  sdk_profile_timers_name_timer(7, "ranked list");
  sdk_profile_timers_start_timer(0);

  i_first_file = parse_command_line(argc, argv, &params);
  input_file_name = argv[i_first_file];

  context = ppr_get_context();
  TRY(ppr_enable_recognition(context));

  TRY(ppr_set_license(context, my_license_id, my_license_key));
  TRY(ppr_set_models_path(context, params.models_path));
  TRY(ppr_set_detection_precision(context, params.detection_precision));

  if (params.detection_precision == PPR_COARSE_PRECISION)
    TRY(ppr_set_coarse_detector_types(context, FRONTAL_FACES_MASK));

  TRY(ppr_set_landmark_detector_type(context, params.landmark_detector_type, PPR_AUTOMATIC_LANDMARKS));
  TRY(ppr_set_num_detection_threads(context, params.num_threads));
  TRY(ppr_set_search_pruning_aggressiveness(context, params.search_pruning));
  TRY(ppr_set_num_recognition_threads(context, params.num_threads));
  TRY(ppr_set_min_size(context, params.min_size));
  TRY(ppr_set_max_size(context, params.max_size));
  TRY(ppr_set_adaptive_min_size(context, params.adaptive_min_size));
  TRY(ppr_set_adaptive_max_size(context, params.adaptive_max_size));
  TRY(ppr_set_detection_threshold(context, params.detection_threshold));
  TRY(ppr_set_frontal_yaw_constraint(context, params.frontal_yaw_constraint));

  sdk_utils_print_context_configuration(stdout, context);

  printf("Initializing...\n");
  
  sdk_profile_timers_start_timer(1);
  TRY(ppr_initialize_context(context));
  sdk_profile_timers_stop_timer(1);

  /********************************************/
  /* Loop Through Images and Create Templates */
  /********************************************/

  sdk_utils_read_file_list(input_file_name, &image_list);
  n_images = image_list.size;

  template_id_list.num_template_ids = n_images;
  template_id_list.template_ids = (int*)malloc(template_id_list.num_template_ids * sizeof(int));
  sdk_utils_check_allocation(template_id_list.template_ids, "template_id_list.template_ids", __FUNCTION__);

  TRY(ppr_create_gallery(context, &gallery));

  for (image_num = 0; image_num < n_images; image_num++)
  {
    /* Acquire image */
    sdk_profile_timers_start_timer(2);
    TRY_RAW_IMAGE(ppr_raw_image_io_read(image_list.filenames[image_num], &raw_image));
    sdk_profile_timers_stop_timer(2);

    TRY(ppr_create_image(raw_image, &image));

    printf("   Processing Image %d: %s => ", image_num, image_list.filenames[image_num]);

    template_id_list.template_ids[image_num] = -1;

    sdk_profile_timers_start_timer(3);
    TRY(ppr_detect_best_frontal_object(context, image, &best_frontal_object));
    sdk_profile_timers_stop_timer(3);
    
    if (best_frontal_object.num_objects > 0)
    {
      TRY(ppr_is_object_suitable_for_recognition(context, best_frontal_object.objects[0], &suitability));

      if (suitability == PPR_OBJECT_SUITABLE_FOR_RECOGNITION)
      {
        sdk_profile_timers_start_timer(4);
        TRY(ppr_extract_template_from_object(context, image, best_frontal_object.objects[0], &curr_template));
        sdk_profile_timers_stop_timer(4);

        TRY(ppr_set_template_string(context, &curr_template, image_list.filenames[image_num]));
        TRY(ppr_copy_template_to_gallery(context, &gallery, curr_template, &id));

        template_id_list.template_ids[image_num] = id;
        ppr_free_template(curr_template);
      }

      ppr_free_object_list(best_frontal_object);
    }

    if (template_id_list.template_ids[image_num] != -1)
      printf("Found valid frontal face.\n");
    else
      printf("No valid frontal face found.\n");

    ppr_raw_image_free(raw_image);
    ppr_free_image(image);
  }

  /*******************/
  /* Compare Results */
  /*******************/

  printf("\n");
  printf("Performing comparisons...\n");
  printf(" - (For each image, showing top %d comparisons with match score greater than %.1f)\n", params.max_comparisons, params.rank_threshold);

  sdk_profile_timers_start_timer(5);
  TRY(ppr_get_self_similarity_matrix(context, gallery, &similarity_matrix));
  sdk_profile_timers_stop_timer(5);

  sdk_profile_timers_start_timer(6);
  TRY(ppr_apply_transitive_matching(context, similarity_matrix, &adjusted_similarity_matrix));
  sdk_profile_timers_stop_timer(6);

  /*****************/
  /* Report Scores */
  /*****************/

  for (i = 0; i < n_images; i++)
  {
    printf("\n");
    printf("===== Image %d: %s:\n", i, image_list.filenames[i]);
    if (template_id_list.template_ids[i] == -1)
    {
      printf("   No valid face found for this image.\n");
    }
    else
    {
      sdk_profile_timers_start_timer(7);
      TRY(ppr_get_ranked_template_list_for_template(context,
                                                    adjusted_similarity_matrix,
                                                    template_id_list.template_ids[i],
                                                    params.max_comparisons,
                                                    params.rank_threshold,
                                                    &ranked_id_list,
                                                    &score_list));
      sdk_profile_timers_stop_timer(7);

      for (j = 0; j < ranked_id_list.num_template_ids; j++)
      {
        TRY(ppr_get_template_string_by_id(context, gallery, ranked_id_list.template_ids[j], name));

        printf("   Match %d: %s  (score = %8.4f)\n", j+1, name, score_list.scores[j]);
      }
      ppr_free_template_id_list(ranked_id_list);
      ppr_free_score_list(score_list);
    }
  }

  /************/
  /* Finalize */
  /************/

  ppr_free_gallery(gallery);
  ppr_free_similarity_matrix(similarity_matrix);
  ppr_free_similarity_matrix(adjusted_similarity_matrix);
  sdk_utils_free_file_list(image_list);
  free(template_id_list.template_ids);

  ppr_release_context(context);
  ppr_finalize_sdk();

  printf("\n");
  printf("Processing complete\n");

  sdk_profile_timers_stop_timer(0);
  sdk_profile_timers_print_timers();

  return EXIT_SUCCESS;
}

/*****************************************************************************/
