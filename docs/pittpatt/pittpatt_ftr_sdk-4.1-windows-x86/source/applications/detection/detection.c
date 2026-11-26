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
 * EXAMPLE PROGRAM : Detection
 *
 * This is an example of using the detector to detect faces in a set of input
 * images.
 *
 * This version includes error checking, the ability to override the
 * detector's default settings, and result overlays.  
 *
 * Use the -help flag for usage information.
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

#define RESULTS_INDENT              0
#define VERBOSE_RESULTS             1
#define COLORS_BASED_ON_CONFIDENCE  1
#define OUTPUT_IMAGE_FORMAT         "jpg"

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
  int object_num;                      /* Index for looping over objects                    */

  char *input_file_name;               /* Name of current input file                        */
  char *image_name;                    /* Name of current image                             */
  sdk_utils_file_list_type image_list; /* List of images to process                         */
  int n_images;                        /* Number of images to process                       */

  ppr_context_type context;            /* SDK detection context                             */
  ppr_raw_image_type raw_image;        /* Raw image read from file                          */
  ppr_image_type image;                /* Image to be submitted to detector                 */
  ppr_object_list_type object_list;    /* List of objects found in input image              */
  ppr_raw_image_type thumbnail;        /* Raw image for thumbnail of detected face          */

  char title[FILENAME_MAX];            /* Name of input image without path or extension     */
  char out_filename[FILENAME_MAX];     /* Name of output file being written                 */

  /****************************/
  /* Configure and Initialize */
  /****************************/

  i_first_file = parse_command_line(argc, argv, &params);

  sdk_profile_timers_init();
  sdk_profile_timers_name_timer(0, "total");
  sdk_profile_timers_name_timer(1, "initialization");
  sdk_profile_timers_name_timer(2, "read image");
  if (params.best_frontal)
    sdk_profile_timers_name_timer(3, "detection (including landmarks)");
  else
    sdk_profile_timers_name_timer(3, "detection");
  sdk_profile_timers_name_timer(4, "landmarks");
  sdk_profile_timers_name_timer(5, "write thumbnails");
  sdk_profile_timers_name_timer(6, "write overlays");
  sdk_profile_timers_name_timer(7, "write binary output");
  sdk_profile_timers_start_timer(0);

  context = ppr_get_context();

  TRY(ppr_set_license(context, my_license_id, my_license_key));
  TRY(ppr_set_models_path(context, params.models_path));
  TRY(ppr_set_detection_precision(context, params.detection_precision));
  TRY(ppr_set_landmark_detector_type(context, params.landmark_detector_type, PPR_MANUAL_LANDMARKS));
  TRY(ppr_set_num_detection_threads(context, params.num_threads));
  TRY(ppr_set_search_pruning_aggressiveness(context, params.search_pruning));
  TRY(ppr_set_min_size(context, params.min_size));
  TRY(ppr_set_max_size(context, params.max_size));
  TRY(ppr_set_adaptive_min_size(context, params.adaptive_min_size));
  TRY(ppr_set_adaptive_max_size(context, params.adaptive_max_size));
  TRY(ppr_set_detection_threshold(context, params.detection_threshold));
  TRY(ppr_set_search_region(context, params.x1, params.y1, params.x2, params.y2));

  sdk_utils_print_context_configuration(stdout, context);

  printf("Initializing...\n");
  
  sdk_profile_timers_start_timer(1);
  TRY(ppr_initialize_context(context));
  sdk_profile_timers_stop_timer(1);

  /**************************/
  /* Read and Submit Images */
  /**************************/

  /* Loop through all input files */
  for (i_input_file = i_first_file ; i_input_file < argc; i_input_file++)
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
      
      sdk_utils_get_file_title(image_name, title);
      
      printf("Reading image '%s' ...\n", image_name);
      
      /* Read input image */
      sdk_profile_timers_start_timer(2);
      TRY_RAW_IMAGE(ppr_raw_image_io_read(image_name, &raw_image));
      sdk_profile_timers_stop_timer(2);

      /* Convert raw image to pittpatt image type */
      TRY(ppr_create_image(raw_image, &image));
      
      printf("Running detector...\n");
      
      /* Submit image to detector */
      if (params.best_frontal)
      {
        sdk_profile_timers_start_timer(3);
        TRY(ppr_detect_best_frontal_object(context, image, &object_list));
        sdk_profile_timers_stop_timer(3);
      }
      else
      {
        sdk_profile_timers_start_timer(3);
        TRY(ppr_detect_objects(context, image, &object_list));
        sdk_profile_timers_stop_timer(3);
      
        /* Submit objects for landmark detection if enabled */
        if (params.landmark_detector_type != PPR_NO_LANDMARK_DETECTOR)
        {
          for (object_num = 0; object_num < object_list.num_objects; object_num++)
          {
            sdk_profile_timers_start_timer(4);
            TRY(ppr_detect_landmarks_from_object(context, image, &object_list.objects[object_num]));
            sdk_profile_timers_stop_timer(4);
          }
        }
      }
      
      /****************/
      /* Write Output */
      /****************/
      
      /* Print the data from the objects to the screen */
      if (params.print)
        sdk_utils_print_object_list(stdout, object_list, RESULTS_INDENT, VERBOSE_RESULTS);

      /* Crop thumbnail of detected objects and write to file */
      if (params.thumbnails)
      {
        printf("Writing thumbnail images...\n");
        sdk_profile_timers_start_timer(5);
        for (object_num = 0; object_num < object_list.num_objects; object_num++)
        {
          TRY(ppr_crop_thumbnail_from_image(context, image, object_list.objects[object_num], &thumbnail));
          sprintf(out_filename, "%s_thumbnail_%03d.%s", title, object_num, OUTPUT_IMAGE_FORMAT);
          TRY_RAW_IMAGE(ppr_raw_image_io_write(out_filename, thumbnail));
          ppr_raw_image_free(thumbnail);
        }
        sdk_profile_timers_stop_timer(5);
      }
      
      /* Overlay object boundaries and landmarks onto image and write to file */
      if (params.overlays)
      {
        printf("Writing output image...\n");
        sdk_profile_timers_start_timer(6);
        sdk_utils_overlay_object_list(&raw_image, object_list, COLORS_BASED_ON_CONFIDENCE);
        sprintf(out_filename, "%s_overlay.%s", title, OUTPUT_IMAGE_FORMAT);
        TRY_RAW_IMAGE(ppr_raw_image_io_write(out_filename, raw_image));
        sdk_profile_timers_stop_timer(6);
      }
      
      /* Write binary output file */
      if (params.binary_output)
      {
        printf("Writing binary output to file...\n");
        sdk_profile_timers_start_timer(7);
        if (params.best_frontal)
          sprintf(out_filename, "%s_best_frontal_object_list.bin", title);
        else
          sprintf(out_filename, "%s_object_list.bin", title);
        TRY(ppr_write_object_list(context, out_filename, object_list, image_name));
        sdk_profile_timers_stop_timer(7);
      }

      /* Free all objects allocated */
      ppr_raw_image_free(raw_image);
      ppr_free_image(image);
      ppr_free_object_list(object_list);
      
      printf("\n");
    }

    if (using_list)
      sdk_utils_free_file_list(image_list);
  }

  /************/
  /* Finalize */
  /************/

  /* Release the detection context since it is no longer needed */
  TRY(ppr_release_context(context));

  /* Finalize the SDK */
  ppr_finalize_sdk();

  printf("Processing complete\n");

  sdk_profile_timers_stop_timer(0);
  sdk_profile_timers_print_timers();

  return EXIT_SUCCESS;
}

/******************************************************************************/
