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
 * EXAMPLE PROGRAM : Create Known Subjects
 *
 * This example takes input in the form of a directory which contains one or
 * more subdirectories that each represent a single subject.  Each
 * subdirectory must have one or more single-frontal-face images of the
 * corresponding subject.
 *
 * A gallery and a subject list are created from the input.  The gallery and
 * subject list are then written to disk as a known-subject gallery.  The
 * known-subject gallery can be read from disk and compared to other galleries
 * to identify people.
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

/******************************************************************************/

int main(int argc, char *argv[])
{
  int d;                                   /* Index for looping through directories                 */
  int f;                                   /* Index for looping through files in a directory        */
  
  parameters_struct params;                /* Parameters from command-line                          */
  int i_first_file;                        /* Index of first file name on command line              */

  ppr_context_type context;                /* SDK detection context                                 */
  ppr_raw_image_type raw_image;            /* Raw image read from file                              */
  ppr_image_type image;                    /* SDK image created from raw image                      */
  ppr_gallery_type gallery;                /* Gallery being created                                 */

  sdk_utils_file_list_type directory_list; /* List of subdirectories                                */
  sdk_utils_file_list_type file_list;      /* List of files within current directory                */

  ppr_subject_list_type subject_list;      /* List of subjects read from directories                */
  ppr_template_id_list_type *p_subject;    /* Pointer to current subject in list                    */

  ppr_object_list_type object_list;        /* List containing the best object detected in the image */
  ppr_object_suitability_type suitability; /* Flag indicating if object is suitable for recognition */
  ppr_template_type curr_template;         /* Template extracted from object                        */
  int template_id;                         /* ID of template after adding to gallery                */

  char directory_name[FILENAME_MAX];       /* Name of directory being processed                     */
  char file_name[FILENAME_MAX];            /* Name of file being processed                          */
  char str[PPR_MAX_TEMPLATE_STRING_SIZE];  /* Template string with subject name added               */
  
  /****************************/
  /* Configure and Initialize */
  /****************************/

  /* Initialize timers for profiling */
  sdk_profile_timers_init();
  sdk_profile_timers_name_timer(0, "total");
  sdk_profile_timers_name_timer(1, "initialization");
  sdk_profile_timers_name_timer(2, "read images");
  sdk_profile_timers_name_timer(3, "detect objects");
  sdk_profile_timers_name_timer(4, "extract templates");
  sdk_profile_timers_name_timer(5, "write gallery");
  sdk_profile_timers_start_timer(0);

  /* Get command-line parameters */
  i_first_file = parse_command_line(argc, argv, &params);

  /* Create and configure context */
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

  /* Initialize context */
  printf("Initializing...\n");
  
  sdk_profile_timers_start_timer(1);
  TRY(ppr_initialize_context(context));
  sdk_profile_timers_stop_timer(1);

  /************************************************************/
  /* Create gallery and subject list from directory structure */
  /************************************************************/
  
  /* Create gallery for subjects */
  TRY(ppr_create_gallery(context, &gallery));

  /* Create subject list for subjects (directories) in input directory */
  sdk_utils_get_directory_list_from_directory(argv[i_first_file], &directory_list);
  subject_list.num_subjects = 0;
  subject_list.subjects = (ppr_template_id_list_type*)malloc(directory_list.size * sizeof(ppr_template_id_list_type));
  subject_list.capacity = directory_list.size;
  sdk_utils_check_allocation(subject_list.subjects, "subject_list.subjects", __FUNCTION__);

  /* Loop through all subjects */
  for (d = 0; d < directory_list.size; d++)
  {
    /* Create new subject */
    printf("Processing subject %s\n", directory_list.filenames[d]);
    sprintf(directory_name, "%s/%s", argv[i_first_file], directory_list.filenames[d]);
    sdk_utils_get_file_list_from_directory(directory_name, &file_list);
    p_subject = &subject_list.subjects[d];
    
    p_subject->num_template_ids = 0;
    p_subject->template_ids = (int*)malloc(file_list.size * sizeof(int));
    p_subject->capacity = file_list.size;
    sdk_utils_check_allocation(p_subject->template_ids, "p_subject->template_ids", __FUNCTION__);

    /* Loop through all images in directory for this subject */
    for (f = 0; f < file_list.size; f++)
    {
      /* Read image */
      sprintf(file_name, "%s/%s", directory_name, file_list.filenames[f]);
      printf("Reading %s\n", file_name);
      sdk_profile_timers_start_timer(2);
      TRY_RAW_IMAGE(ppr_raw_image_io_read(file_name, &raw_image));
      sdk_profile_timers_stop_timer(2);
      TRY(ppr_create_image(raw_image, &image));

      /* Detect best frontal object in image */
      printf("Detecting\n");
      sdk_profile_timers_start_timer(3);
      TRY(ppr_detect_best_frontal_object(context, image, &object_list));
      sdk_profile_timers_stop_timer(3);
      if (params.print_objects)
        sdk_utils_print_object_list(stdout, object_list, 0, 1);

      if (object_list.num_objects > 0)
      {
        TRY(ppr_is_object_suitable_for_recognition(context, object_list.objects[0], &suitability));

        if (suitability == PPR_OBJECT_SUITABLE_FOR_RECOGNITION)
        {
          printf("Getting template\n");
          sdk_profile_timers_start_timer(4);
          TRY(ppr_extract_template_from_object(context, image, object_list.objects[0], &curr_template));
          sdk_profile_timers_stop_timer(4);

          TRY(ppr_get_template_string(context, curr_template, str));
          sdk_utils_add_item_to_template_string(str, "#NAME", directory_list.filenames[d]);
          sdk_utils_add_item_to_template_string(str, "#SOURCE", file_name);
          TRY(ppr_set_template_string(context, &curr_template, str));
          TRY(ppr_copy_template_to_gallery(context, &gallery, curr_template, &template_id));
          
          p_subject->template_ids[p_subject->num_template_ids] = template_id;
          p_subject->num_template_ids++;
          
          ppr_free_template(curr_template);
        }
        else
        {
          printf("WARNING: Detected object is not suitable for recognition\n");
        }
      }
      else
      {
        printf("WARNING: No object detected\n");
      }
      
      ppr_free_object_list(object_list);
      ppr_raw_image_free(raw_image);
      ppr_free_image(image);
    }
    printf("\n");

    sdk_utils_free_file_list(file_list);

    /* Do not keep subject if no templates were added to it */
    if (p_subject->num_template_ids > 0)
      subject_list.num_subjects++;
    else
      free(p_subject->template_ids);
  }

  /****************/
  /* Write output */
  /****************/

  sdk_profile_timers_start_timer(5);
  TRY(ppr_write_gallery_with_subject_list(context, argv[i_first_file + 1], gallery, subject_list));
  sdk_profile_timers_stop_timer(5);
  
  /************/
  /* Finalize */
  /************/

  sdk_utils_free_file_list(directory_list);
  ppr_free_gallery(gallery);
  for (d = 0; d < subject_list.num_subjects; d++)
    free(subject_list.subjects[d].template_ids);
  free(subject_list.subjects);

  ppr_release_context(context);
  ppr_finalize_sdk();

  printf("\n");
  printf("Processing complete\n");

  sdk_profile_timers_stop_timer(0);
  sdk_profile_timers_print_timers();
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
