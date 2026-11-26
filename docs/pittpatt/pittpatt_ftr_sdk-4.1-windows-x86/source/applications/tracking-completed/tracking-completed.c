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
 * EXAMPLE APPLICATION : Tracking Using Completed Results
 *
 * This example uses the tracker to process a video sequence, extracting results
 * after tracks have come to completion.  See the 'Tracking Using Completed Results'
 * framework in the Tracking User's Guide for more information.
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

#define PRINT_OBJECTS_IN_TRACK  0
#define MIN_OVERLAY_CONFIDENCE  2
#define INSTANCE_DIRECTORY      "." 
#define OUTPUT_IMAGE_FORMAT     "jpg"
#define OUTPUT_VIDEO_FORMAT     "mp4"

/*****************************************************************************
 * Functions
 *****************************************************************************/

int main(int argc, char *argv[])
{
  int frame_num;                                      /* Index for looping over frames              */

  parameters_struct params;                           /* Parameters from command-line               */
  int i_first_file;                                   /* Index of first file name on command line   */
  int i_input_file;                                   /* Index for looping through input files      */
  char extension[FILENAME_MAX];                       /* Extension of input file                    */
  int using_video;                                    /* Flag set when using video vs. image list   */
  char *input_file_name;                              /* Name of image list input file              */
  sdk_utils_file_list_type image_list;                /* List of images to submit to tracker        */

  int n_frames;                                       /* Number of frames to be processed           */
  ppr_video_io_type video;                            /* Video to be processed                      */

  ppr_context_type context;                           /* SDK tracking context                       */
  ppr_raw_image_type raw_image;                       /* Raw image read from file                   */
  ppr_image_type image;                               /* Image to be submitted to tracker           */
  ppr_completed_track_list_type completed_track_list; /* List of all tracks found                   */

  char title[FILENAME_MAX];                           /* Input file name without path and extension */
  char output_file_name[FILENAME_MAX];                /* Name of file being written                 */

  /****************************/
  /* Configure and Initialize */
  /****************************/

  sdk_profile_timers_init();
  sdk_profile_timers_name_timer(0, "total");
  sdk_profile_timers_name_timer(1, "initialization");
  sdk_profile_timers_name_timer(2, "read frame");
  sdk_profile_timers_name_timer(3, "detect and track");
  sdk_profile_timers_name_timer(4, "write overlays");
  sdk_profile_timers_name_timer(5, "write binary output");
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
    TRY(ppr_enable_visual_attention(context));

  TRY(ppr_set_num_visual_instances(context, params.num_visual_instances));

  if (params.use_shot_boundary_detection)
  {
    TRY(ppr_enable_shot_boundary_detection(context));
    TRY(ppr_set_shot_boundary_threshold(context, params.shot_boundary_threshold));
  }

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
      
      printf("Processing Frame[%d]\n", frame_num);
      
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
      
      /**************************************************/
      /* Write Completed Tracks Periodically If Enabled */
      /**************************************************/
      
      if (params.num_frames_between_extracts != 0)
      {
        /* Extract completed tracks every N frames */
        if ((frame_num + 1) % params.num_frames_between_extracts == 0)
        {
          printf("\n");
          printf("*** Getting Completed Tracks (if any) ***\n");
          printf("\n");
        
          /* Get tracks that have been completed to this point */
          TRY(ppr_get_completed_tracks(context, &completed_track_list));
        
          /* Display data from completed tracks */
          if (params.print)
          {
            sdk_utils_print_completed_track_list(stdout, completed_track_list, PRINT_OBJECTS_IN_TRACK, RESULTS_INDENT,
                                                 VERBOSE_RESULTS);
          }

          if (params.binary_output)
          {
            /* Write completed track list binary file */
            printf("Writing binary output file...\n");
            sdk_profile_timers_start_timer(5);
            sdk_utils_get_file_title(input_file_name, title);
            sprintf(output_file_name, "%s_frame_%06d_completed_track_list.bin", title, frame_num);
            TRY(ppr_write_completed_track_list(context, output_file_name, completed_track_list, input_file_name));
            sdk_profile_timers_stop_timer(5);
          }

          /* Write visual instances from each track */
          sdk_utils_write_visual_instances(INSTANCE_DIRECTORY, OUTPUT_IMAGE_FORMAT, completed_track_list);
        
          /* Free completed track list */
          ppr_free_completed_track_list(completed_track_list);
        }
      }

      /******************************/
      /* Prepare for Next Iteration */
      /******************************/

      /* Free image */
      ppr_raw_image_free(raw_image);
      ppr_free_image(image);

      /* Increment frame */
      frame_num++;

      if (using_video)
        TRY_VIDEO(ppr_video_io_step_forward(video));
    }

    /************************/
    /* Get Remaining Tracks */
    /************************/

    /* Finalize all remaining tracks internal to the tracker */
    printf("\n");
    printf("*** Forcing completion of remaining tracks (if any) ***\n");
    printf("\n");
    TRY(ppr_finalize_current_tracks(context));

    /* Get all remaining tracks from the tracker */
    TRY(ppr_get_completed_tracks(context, &completed_track_list));

    /****************/
    /* Write Output */
    /****************/

    /* Display data from all remaining tracks */
    if (params.print)
    {
      sdk_utils_print_completed_track_list(stdout, completed_track_list, PRINT_OBJECTS_IN_TRACK, RESULTS_INDENT,
                                           VERBOSE_RESULTS);
    }

    /* Write visual instances from each track */
    sdk_utils_write_visual_instances(INSTANCE_DIRECTORY, OUTPUT_IMAGE_FORMAT, completed_track_list);

    if (params.overlays && (params.num_frames_between_extracts == 0))
    {
      /* Write annotated images */
      printf("Writing annotated images to disk...\n");
      sdk_profile_timers_start_timer(4);
      if (using_video)
      {
        sdk_utils_overlay_completed_track_list_on_video(input_file_name, completed_track_list, MIN_OVERLAY_CONFIDENCE,
                                                        params.frame_numbers, OUTPUT_VIDEO_FORMAT);
      }
      else
      {
        sdk_utils_overlay_completed_track_list_on_image_list(image_list, completed_track_list, MIN_OVERLAY_CONFIDENCE,
                                                             params.frame_numbers, OUTPUT_IMAGE_FORMAT);
      }
      sdk_profile_timers_stop_timer(4);
    }

    if (params.binary_output)
    {
      /* Write completed track list binary file */
      printf("Writing binary output file...\n");
      sdk_profile_timers_start_timer(5);
      sdk_utils_get_file_title(input_file_name, title);
      if (params.num_frames_between_extracts == 0)
        sprintf(output_file_name, "%s_completed_track_list.bin", title);
      else
        sprintf(output_file_name, "%s_frame_%06d_completed_track_list.bin", title, frame_num);
      TRY(ppr_write_completed_track_list(context, output_file_name, completed_track_list, input_file_name));
      sdk_profile_timers_stop_timer(5);
    }
    
    /********/
    /* Free */
    /********/

    /* Free the completed tracks */
    ppr_free_completed_track_list(completed_track_list);

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

/*****************************************************************************/
