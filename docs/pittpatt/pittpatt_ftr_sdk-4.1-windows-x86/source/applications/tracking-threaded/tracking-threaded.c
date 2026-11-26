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
 * EXAMPLE APPLICATION : Frame-level threading (tracking)
 *
 * This example shows how to use frame-level threading to accomplish a tracking problem
 *
 * Each thread gets a frame from a shared frame_source, does detection on the frame, and then
 * must submit the results of detection *in order*. The in order submission of frames to the tracker
 * is done via a shared tracker. The shared tracker knows which frame needs to be submitted (e.g,
 * frame #10). If the submitted frame is not the correct frame, it adds it the list of frames to-be-added
 * and quits. Once the frame needed is submitted, it will then add all the sequential frames in the to-be-added
 * list. The to-be-added list is implemented using a mutex and a singly-linked list.
 *
 * Use the -help flag for usage information.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "pittpatt_ftr_sdk.h"
#include "pittpatt_license.h"
#include "pittpatt_raw_image_io.h"
#include "pittpatt_video_io.h"
#include "sdk_utils.h"
#include "sdk_profile_timers.h"

#include "parse_command_line.h"
#include "frame_source.h"

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

/* elements of our results linked list */
struct single_frame_results_element {
  ppr_image_type frame;
  ppr_object_list_type result;
  int frame_number;
  struct single_frame_results_element *next_result;
};
typedef struct single_frame_results_element frame_result_type;

/* the shared tracker (among worker threads) and it's associated state */
struct shared_tracker_struct {
  ppr_context_type context;
  int next_frame_to_be_added;
  frame_result_type *unadded_frames;
  pthread_mutex_t tracker_mutex;
};
typedef struct shared_tracker_struct shared_tracker_type;

/* data passed to threads */
struct thread_data_struct {
  shared_tracker_type *tracker;
  frame_source_type *source;
};
typedef struct thread_data_struct thread_data_type;

/**************************************************************
 * Functions
 **************************************************************/

shared_tracker_type init_shared_tracker(ppr_context_type context)
{
  shared_tracker_type answer;  
  answer.context = context;
  answer.next_frame_to_be_added = 0;
  answer.unadded_frames = 0;
  pthread_mutex_init(&answer.tracker_mutex, NULL);
  return answer;
}

/******************************************************************************/

/* insert a new results into our ordered (by frame number) linked list of unadded frames */

void insert_new_result(shared_tracker_type *tracker, frame_result_type *new_result)
{
  frame_result_type *curr;
  curr = tracker->unadded_frames;
  
  if (curr == 0 || new_result->frame_number < curr->frame_number) /* does it go at the beginning? */
  {
    new_result->next_result = curr;
    tracker->unadded_frames = new_result;
    return;
  }

  while (curr->next_result)
  {
    if (new_result->frame_number < curr->next_result->frame_number)
    {
      new_result->next_result = curr->next_result;
      curr->next_result = new_result;
      return;
    }
    curr = curr->next_result;
  }

  /* goes at the end */  
  curr->next_result = new_result;
}

/******************************************************************************/

/* if the head of the unadded list is the frame we've been waiting for, submit it (and continue doing so) */
void update_tracker(shared_tracker_type *tracker)
{
  /* if first unadded frame is the one we've been waiting for, submit it, remove it, free it
     (and keep going until it's not) */
  while (tracker->unadded_frames && tracker->unadded_frames->frame_number == tracker->next_frame_to_be_added)
  {
    frame_result_type *single_result = tracker->unadded_frames;
        
    if (single_result->frame_number % 10 == 0)
      printf("finished frame %d\n",single_result->frame_number);

    tracker->next_frame_to_be_added++;    
    tracker->unadded_frames = tracker->unadded_frames->next_result;
    
    ppr_track_objects(tracker->context, single_result->frame, single_result->result);
    
    ppr_free_object_list(single_result->result);
    ppr_free_image(single_result->frame);
    free(single_result);
  }
}

/******************************************************************************/

/* This functions handles the sequential submissions of results to the tracker.
   It adds the new_result to the unadded frames list and then submits as many frames from
   the unadded list as it can. This is all done inside of protected mutexes (note: this
   synchronization is overly conversative but the overhead should be minimal).
*/
void submit_to_shared_tracker(shared_tracker_type *tracker, frame_result_type *new_result)
{
  pthread_mutex_lock(&tracker->tracker_mutex);

  insert_new_result(tracker, new_result);

  update_tracker(tracker);
  
  pthread_mutex_unlock(&tracker->tracker_mutex);
}

/******************************************************************************/

/* the body of the actual threads that do the detection & tracking */
void* worker_thread(void *data)
{
  int frame_num;
  shared_tracker_type *tracker = ((thread_data_type*)data)->tracker;
  frame_source_type *source = ((thread_data_type*)data)->source;
  
  while (1)
  {
    frame_result_type *answer;
    ppr_image_type image = get_next_frame(source, &frame_num);
    
    if (frame_num < 0)  /* no more input, time to quit */
      break;

    answer = malloc(sizeof(frame_result_type));
    answer->frame = image;
    answer->frame_number = frame_num;
    answer->next_result = 0;
    
    /* Submit image to detect, add results to linked list */
    TRY(ppr_detect_objects(tracker->context, image, &answer->result));

    /* this function handles sequential submission to tracker (and freeing of data) */
    submit_to_shared_tracker(tracker, answer);
  }

  ppr_finalize_thread();
  
  return 0;
}

/******************************************************************************/

int main(int argc, char *argv[])
{
  parameters_struct params;                           /* Parameters from command-line               */
  int i_first_file;                                   /* Index of first file name on command line   */

  int t;
  char *input_file_name;                              /* Name of image list input file              */

  ppr_context_type context;
  ppr_completed_track_list_type completed_track_list; /* List of all tracks found                   */

  char title[FILENAME_MAX];                           /* Input file name without path and extension */
  char output_file_name[FILENAME_MAX];                /* Name of file being written                 */

  pthread_t *threads;
  pthread_attr_t attr;

  frame_source_type frame_source;
  shared_tracker_type shared_tracker;
  thread_data_type shared_thread_data;
  
  /****************************/
  /* Configure and Initialize */
  /****************************/

  sdk_profile_timers_init();
  sdk_profile_timers_name_timer(0, "total");
  sdk_profile_timers_name_timer(1, "initialization");
  sdk_profile_timers_name_timer(2, "detection & tracking");
  sdk_profile_timers_name_timer(3, "unused");
  sdk_profile_timers_name_timer(4, "write overlays");
  sdk_profile_timers_name_timer(5, "write binary output");
  sdk_profile_timers_start_timer(0);
  sdk_profile_timers_start_timer(1);
  
  i_first_file = parse_command_line(argc, argv, &params);

  context = ppr_get_context();
  TRY(ppr_enable_tracking(context));
  
  TRY(ppr_set_license(context, my_license_id, my_license_key));
  TRY(ppr_set_models_path(context, params.models_path));
  TRY(ppr_set_detection_precision(context, params.detection_precision));
  TRY(ppr_set_landmark_detector_type(context, params.landmark_detector_type, PPR_AUTOMATIC_LANDMARKS));
  TRY(ppr_set_num_detection_threads(context, 1)); /* NOTE: we are doing threading, not doing it internally */
  TRY(ppr_set_search_pruning_aggressiveness(context, params.search_pruning));
  TRY(ppr_set_tracking_cutoff(context, params.tracking_cutoff));

  /* Visual attention doesn't work with frame-threaded tracking */
  /*
    if (params.use_visual_attention)
    TRY(ppr_enable_visual_attention(context));
  */

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
  
  TRY(ppr_initialize_context(context));
  input_file_name = argv[i_first_file];

  frame_source = frame_source_open(input_file_name);
  
  /*********************************/
  /* Delegate detection to threads */
  /*********************************/

  /* Initialize mutex and attributes */
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  
  threads = malloc(sizeof(pthread_t) * params.num_threads);

  shared_tracker = init_shared_tracker(context);
  
  shared_thread_data.tracker = &shared_tracker;
  shared_thread_data.source = &frame_source;
      
  sdk_profile_timers_stop_timer(1);
  sdk_profile_timers_start_timer(2);
  
  printf("Starting worker threads...\n");
  /* Create threads */
  for (t = 0; t < params.num_threads; t++)
  {
    pthread_create(&threads[t], &attr, worker_thread, (void *) &shared_thread_data);
  }
  

  pthread_attr_destroy(&attr);
  for (t = 0; t < params.num_threads; t++)
  {
    int error_code;

    error_code = pthread_join(threads[t], NULL);
    if (error_code)
    {
      fprintf(stderr, "error joining pthread (err: %d)\n", error_code);
      exit(EXIT_FAILURE);
    }
  }

  free(threads);
  
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

  sdk_profile_timers_stop_timer(2);
  
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
    if (frame_source.using_video)
    {
      sdk_utils_overlay_completed_track_list_on_video(input_file_name, completed_track_list, MIN_OVERLAY_CONFIDENCE,
                                                      params.frame_numbers, OUTPUT_VIDEO_FORMAT);
    }
    else
    {
      sdk_utils_overlay_completed_track_list_on_image_list(frame_source.image_list, completed_track_list,
                                                           params.frame_numbers, MIN_OVERLAY_CONFIDENCE,
                                                           OUTPUT_IMAGE_FORMAT);
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
      sprintf(output_file_name, "%s_frame_%06d_completed_track_list.bin", title, frame_source.frame_num);
    TRY(ppr_write_completed_track_list(context, output_file_name, completed_track_list, input_file_name));
    sdk_profile_timers_stop_timer(5);
  }
  
  /********/
  /* Free */
  /********/
  
  /* Free the completed tracks */
  ppr_free_completed_track_list(completed_track_list);

  frame_source_close(&frame_source);

  TRY(ppr_reset_tracker(context));
  
  
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
