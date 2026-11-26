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
 * EXAMPLE PROGRAM : Create Gallery
 *
 * This example reads a list of images, detects all of the faces in all of the
 * images, and creates a template for each face.  All templates are added to a
 * gallery.  The gallery is then written to disk.
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
 * Local Function Prototypes
 *****************************************************************************/

static void copy_image_templates_to_gallery(
  ppr_context_type context,
  parameters_struct params,
  const char *image_name,
  ppr_raw_image_type raw_image,
  ppr_gallery_type *gallery);

/*****************************************************************************
 * Functions
 *****************************************************************************/

int main(int argc, char *argv[])
{
  parameters_struct params;            /* Parameters from command-line                      */
  int i_first_file;                    /* Index of first file name on command line          */
  int i_input_file;                    /* Index for looping through input files             */
  char extension[FILENAME_MAX];        /* Extension of current input file                   */
  int using_list;                      /* Flag indicating that current input file is a list */

  int image_num;                       /* Index for looping over images                     */
  char *input_file_name;               /* Name of current input file                        */
  char *image_name;                    /* Name of current image                             */
  sdk_utils_file_list_type image_list; /* List of input images                              */
  int n_images;                        /* Number of images to process                       */

  ppr_context_type context;            /* SDK detection context                             */
  ppr_raw_image_type raw_image;        /* Raw image read from file                          */
  ppr_gallery_type gallery;            /* Gallery being created                             */

  /****************************/
  /* Configure and Initialize */
  /****************************/

  sdk_profile_timers_init();
  sdk_profile_timers_name_timer(0, "total");
  sdk_profile_timers_name_timer(1, "initialization");
  sdk_profile_timers_name_timer(2, "read frame");
  sdk_profile_timers_name_timer(3, "detect objects");
  sdk_profile_timers_name_timer(4, "extract templates");
  sdk_profile_timers_name_timer(5, "write gallery");
  sdk_profile_timers_start_timer(0);

  /* Get command-line options */
  i_first_file = parse_command_line(argc, argv, &params);

  context = ppr_get_context();
  TRY(ppr_enable_recognition(context));

  TRY(ppr_set_license(context, my_license_id, my_license_key));
  TRY(ppr_set_models_path(context, params.models_path));
  TRY(ppr_set_detection_precision(context, params.detection_precision));
  TRY(ppr_set_landmark_detector_type(context, params.landmark_detector_type, PPR_AUTOMATIC_LANDMARKS));
  TRY(ppr_set_num_detection_threads(context, params.num_threads));
  TRY(ppr_set_num_recognition_threads(context, params.num_threads));
  TRY(ppr_set_search_pruning_aggressiveness(context, params.search_pruning));
  TRY(ppr_set_min_size(context, params.min_size));
  TRY(ppr_set_max_size(context, params.max_size));
  TRY(ppr_set_adaptive_min_size(context, params.adaptive_min_size));
  TRY(ppr_set_adaptive_max_size(context, params.adaptive_max_size));
  TRY(ppr_set_detection_threshold(context, params.detection_threshold));
  TRY(ppr_set_frontal_yaw_constraint(context, params.frontal_yaw_constraint));
  TRY(ppr_set_template_extraction_type(context, params.extraction_type));

  sdk_utils_print_context_configuration(stdout, context);

  printf("Initializing...\n");
  
  sdk_profile_timers_start_timer(1);
  TRY(ppr_initialize_context(context));
  sdk_profile_timers_stop_timer(1);

  TRY(ppr_create_gallery(context, &gallery));

  /**************************/
  /* Read and Submit Images */
  /**************************/

  for (i_input_file = i_first_file + 1 ; i_input_file < argc; i_input_file++)
  {
    input_file_name = argv[i_input_file];

    /* Determine if input file is image or image list */
    sdk_utils_get_file_extension(input_file_name, extension);
    if (strcmp(extension, "txt") == 0)
      using_list = 1;
    else
      using_list = 0;

    if (using_list)
    {
      sdk_utils_read_file_list(input_file_name, &image_list);
      n_images = image_list.size;
    }
    else
    {
      n_images = 1;
    }

    for (image_num = 0; image_num < n_images; image_num++)
    {
      if (using_list)
        image_name = image_list.filenames[image_num];
      else
        image_name = input_file_name;

      /* Acquire image */
      printf("\n");
      printf("Reading image %s from disk...\n", image_name);
      sdk_profile_timers_start_timer(2);
      TRY_RAW_IMAGE(ppr_raw_image_io_read(image_name, &raw_image));
      sdk_profile_timers_stop_timer(2);

      /* Add templates from current image to gallery */
      copy_image_templates_to_gallery(context, params, image_name, raw_image, &gallery);

      /* Free image that was read */
      ppr_raw_image_free(raw_image);
    }

    if (using_list)
      sdk_utils_free_file_list(image_list);
  }

  /*****************/
  /* Write Gallery */
  /*****************/

  printf("\n");
  printf("Gallery:\n");
  printf("\n");
  sdk_utils_print_gallery(stdout, context, gallery, params.print_relationships);
  printf("\n");

  printf("Writing gallery to disk...\n");

  sdk_profile_timers_start_timer(5);
  TRY(ppr_write_gallery(context, argv[i_first_file], gallery));
  sdk_profile_timers_stop_timer(5);

  /************/
  /* Finalize */
  /************/

  ppr_free_gallery(gallery);

  ppr_release_context(context);
  ppr_finalize_sdk();

  printf("\n");
  printf("Processing complete\n");

  sdk_profile_timers_stop_timer(0);
  sdk_profile_timers_print_timers();

  return EXIT_SUCCESS;
}

/*****************************************************************************/

/*
 * Adds all templates that can be extracted from the raw image to an existing gallery
 */
static void copy_image_templates_to_gallery(
  ppr_context_type context,
  parameters_struct params,
  const char *image_name,
  ppr_raw_image_type raw_image,
  ppr_gallery_type *gallery)
{
  int i;
  int j;

  ppr_image_type image;
  ppr_object_list_type object_list;
  ppr_template_type curr_template;
  ppr_object_suitability_type suitability;

  int *added_template_ids;
  int num_added_ids;
  
  char image_filename[FILENAME_MAX];
  char str[PPR_MAX_TEMPLATE_STRING_SIZE];

  sdk_utils_get_file_filename(image_name, image_filename);

  TRY(ppr_create_image(raw_image, &image));

  /* Detect faces in image */
  sdk_profile_timers_start_timer(3);
  TRY(ppr_detect_objects(context, image, &object_list));
  sdk_profile_timers_stop_timer(3);      
  if (params.print_objects)
    sdk_utils_print_object_list(stdout, object_list, 0, 1);

  /* Create template for each face and add to gallery */
  added_template_ids = (int*)malloc(object_list.num_objects * sizeof(int));
  sdk_utils_check_allocation(added_template_ids, "added_template_ids", "copy_image_templates_to_gallery");
  num_added_ids = 0;
  for (i = 0; i < object_list.num_objects; i++)
  {
    TRY(ppr_is_object_suitable_for_recognition(context, object_list.objects[i], &suitability));

    if (suitability == PPR_OBJECT_SUITABLE_FOR_RECOGNITION)
    {
      sdk_profile_timers_start_timer(4);
      TRY(ppr_extract_template_from_object(context, image, object_list.objects[i], &curr_template));
      sdk_profile_timers_stop_timer(4);

      TRY(ppr_get_template_string(context, curr_template, str));
      sdk_utils_add_item_to_template_string(str, "#SOURCE", image_filename);
      TRY(ppr_set_template_string(context, &curr_template, str));

      TRY(ppr_copy_template_to_gallery(context, gallery, curr_template, &added_template_ids[num_added_ids]));
      num_added_ids++;
      
      ppr_free_template(curr_template);
    }
  }

  /* If subjects are unique within an image, set template relationships as different */
  if (params.subjects_unique_within_image)
  {
    for (i = 0; i < num_added_ids; i++)
    {
      for (j = i + 1; j < num_added_ids; j++)
      {
        TRY(ppr_set_template_relationship(context, gallery, added_template_ids[i], added_template_ids[j],
                                          PPR_RELATIONSHIP_DIFFERENT_SUBJECTS));
      }
    }
  }

  printf("Templates extracted: %d\n", num_added_ids);
        
  ppr_free_image(image);
  ppr_free_object_list(object_list);
  free(added_template_ids);
}

/******************************************************************************/
