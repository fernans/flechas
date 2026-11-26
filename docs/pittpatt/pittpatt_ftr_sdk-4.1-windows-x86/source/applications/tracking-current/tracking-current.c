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
 * EXAMPLE : Tracking Using Current Results
 *
 * This example demonstrates how to acquire frame-by-frame data from the tracker
 * such that the results can be displayed in real-time.  It reads a video
 * sequence and optionally generates visualization data after each frame (in
 * this case, it saves annotated images to disk).  See the 'Tracking Using
 * Current Results' framework in the Tracking User's Guide for more information.
 *
 * Please note that results are printed to the screen each time through the
 * main loop and an annotated image can be written to disk each time through
 * the main loop using the -overlays flag.  However, please be aware that any
 * output occurring within the main loop will slow down this example.  Printing
 * can be disabled with the -no_print flag.
 *
 * Use the -help flag for usage information.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pittpatt_ftr_sdk.h"
#include "pittpatt_license.h"

#include "pittpatt_raw_image_io.h"
#include "pittpatt_video_io.h"

#include "parse_command_line.h"
#include "sdk_utils.h"
#include "sdk_profile_timers.h"

/*****************************************************************************
 * Constants
 *****************************************************************************/

#define RESULTS_INDENT          0
#define VERBOSE_RESULTS         0
#define MIN_OVERLAY_CONFIDENCE  2
#define OUTPUT_IMAGE_FORMAT     "jpg"

/*****************************************************************************
 * Functions
 *****************************************************************************/

int main(int argc, char *argv[])
{
  int frame_num;                                    /* Index for looping over frames                 */

  parameters_struct params;                         /* Parameters from command-line                  */
  int i_first_file;                                 /* Index of first file name on command line      */
  int i_input_file;                                 /* Index for looping through input files         */
  char extension[FILENAME_MAX];                     /* Extension of input file                       */
  int using_video;                                  /* Flag set when using video vs. image list      */
  char *input_file_name;                            /* Name of image list input file                 */
  sdk_utils_file_list_type image_list;              /* List of images to submit to tracker           */

  int n_frames;                                     /* Number of frames to be processed              */
  ppr_video_io_type video;                          /* Video to be processed                         */

  ppr_context_type context;                         /* SDK tracking context                          */
  ppr_raw_image_type raw_image;                     /* Raw image read from file                      */
  ppr_image_type image;                             /* Image to be submitted to tracker              */
  ppr_tracked_object_list_type tracked_object_list; /* List of objects found in current frame        */

  char title[FILENAME_MAX];                         /* Name of input file without extension and path */
  char out_filename[FILENAME_MAX];                  /* Name of output image                          */

  /****************************/
  /* Configure and Initialize */
  /****************************/

  sdk_profile_timers_init();
  sdk_profile_timers_name_timer(0, "total");
  sdk_profile_timers_name_timer(1, "initialization");
  sdk_profile_timers_name_timer(2, "read frame");
  sdk_profile_timers_name_timer(3, "detect and track");
  sdk_profile_timers_name_timer(4, "write overlay");
  sdk_profile_timers_name_timer(4, "write binary output");
  sdk_profile_timers_start_timer(0);

  i_first_file = parse_command_line(argc, argv, &params);

  context = ppr_get_context();
  TRY(ppr_enable_tracking(context));
  
  TRY(ppr_set_license(context, my_license_id, my_license_key));
  TRY(ppr_set_models_path(context, params.models_path));
  TRY(ppr_set_detection_precision(context, params.detection_precision));
  TRY(ppr_set_landmark_detector_type(context, params.landmark_detector_type, PPR_AUTOMATIC_LANDMARKS));
  TRY(ppr_set_num_detection_threads(context, params.num_threads));
  TRY(ppr_set_search_pruning_aggressiveness(context, params.search_pruning));
  TRY(ppr_set_tracking_cutoff(context, params.tracking_cutoff));

  if (params.use_visual_attention)
    TRY_VIDEO(ppr_enable_visual_attention(context));

  TRY(ppr_set_min_size(context, params.min_size));
  TRY(ppr_set_max_size(context, params.max_size));

  sdk_utils_print_context_configuration(stdout, context);

  printf("Initializing...\n");
  
  sdk_profile_timers_start_timer(1);
  TRY(ppr_initialize_context(context));
  sdk_profile_timers_stop_timer(1);

  /**********************/
  /* Loop Through Input */
  /**********************/

  for (i_input_file = i_first_file ; i_input_file < argc; i_input_file++)
  {
    input_file_name = argv[i_input_file];
    sdk_utils_get_file_title(input_file_name, title);

    sdk_utils_get_file_extension(input_file_name, extension);
    if (strcmp(extension, "txt") == 0)
      using_video = 0;
    else
      using_video = 1;

    if (using_video)
    {
      /* Open video */
      TRY_VIDEO(ppr_video_io_open(&video, input_file_name));
    }
    else
    {
      /* Open list of images */
      sdk_utils_read_file_list(input_file_name, &image_list);
      n_frames = image_list.size;
      
      printf("\n");
      printf("Number of frames to process: %d\n", n_frames);
      printf("\n");
    }
    
    /* Loop through all frames */
    frame_num = 0;
    while ((!using_video && frame_num < n_frames) ||
           (using_video && ppr_video_io_is_frame_available(video)))
    {
      /*************************/
      /* Read and Submit Image */
      /*************************/
      
      printf("****************\n");
      printf(" FRAME[%d]\n", frame_num);
      printf("****************\n");
      
      /* Read next frame */
      sdk_profile_timers_start_timer(2);
      if (using_video)
      {
        TRY_VIDEO(ppr_video_io_get_frame(video, &raw_image));
      }
      else
      {
        TRY_RAW_IMAGE(ppr_raw_image_io_read(image_list.filenames[frame_num], &raw_image));
      }
      sdk_profile_timers_stop_timer(2);
      
      /* Convert raw image to pittpatt image type */
      TRY(ppr_create_image(raw_image, &image));
      
      /* Submit image to tracker */
      sdk_profile_timers_start_timer(3);
      TRY(ppr_detect_and_track_objects(context, image));
      sdk_profile_timers_stop_timer(3);
      
      /* Get this frame's objects from the tracker */
      TRY(ppr_get_current_frame_objects(context, &tracked_object_list));
            
      /****************/
      /* Write Output */
      /****************/

      /* Print the data from this frame's objects to the screen */
      if (params.print)
        sdk_utils_print_tracked_object_list(stdout, tracked_object_list, RESULTS_INDENT, VERBOSE_RESULTS);
      
      /* Overlay object boundaries and landmarks onto image and write to file if enabled */
      if (params.overlays)
      {
        sdk_profile_timers_start_timer(4);
        sdk_utils_overlay_tracked_object_list(&raw_image, tracked_object_list, MIN_OVERLAY_CONFIDENCE);
        if (using_video)
          sprintf(out_filename, "%s_%06d_current_tracks.%s", title, frame_num, OUTPUT_IMAGE_FORMAT);
        else
          sprintf(out_filename, "%s_current_tracks.%s", image_list.filenames[frame_num], OUTPUT_IMAGE_FORMAT);

        if (params.frame_numbers)
          sdk_utils_overlay_frame_number(&raw_image, frame_num);
        
        TRY_RAW_IMAGE(ppr_raw_image_io_write(out_filename, raw_image));
        sdk_profile_timers_stop_timer(4);
      }

      if (params.binary_output)
      {
        /* Write tracked object list list binary file */
        printf("Writing binary output file...\n");
        sdk_profile_timers_start_timer(5);
        sprintf(out_filename, "%s_frame_%06d_tracked_object_list.bin", title, frame_num);
        TRY(ppr_write_tracked_object_list(context, out_filename, tracked_object_list, input_file_name));
        sdk_profile_timers_stop_timer(5);
      }

      /******************************/
      /* Prepare for Next Iteration */
      /******************************/

      /* Free image */
      ppr_raw_image_free(raw_image);
      ppr_free_image(image);

      /* Free remaining objects allocated this frame */
      ppr_free_tracked_object_list(tracked_object_list);

      /* Free any completed tracks internal to the tracker since we will not be using them in this scenario */
      TRY(ppr_discard_unreturned_completed_tracks(context));

      /* Increment frame */
      frame_num++;

      if (using_video)
        TRY(ppr_video_io_step_forward(video));
    }

    /********/
    /* Free */
    /********/

    if (using_video)
    {
      TRY_VIDEO(ppr_video_io_close(video));
    }
    else
    {
      sdk_utils_free_file_list(image_list);
    }

    TRY(ppr_reset_tracker(context));
  }

  /************/
  /* Finalize */
  /************/

  /* Release the tracking context since it is no longer needed */
  TRY(ppr_release_context(context));

  /* Finalize the SDK */
  ppr_finalize_sdk();

  printf("Processing complete\n");

  sdk_profile_timers_stop_timer(0);
  sdk_profile_timers_print_timers();

  return EXIT_SUCCESS;
}

/******************************************************************************/
