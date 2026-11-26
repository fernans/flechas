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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pittpatt_ftr_sdk.h"

#include "parse_command_line.h"
#include "sdk_utils.h"
#include "sdk_cmd_line_utils.h"
#include "models_path.h"

/*****************************************************************************
 * Constants
 *****************************************************************************/

#define DEFAULT_MODELS_PATH                  MODELS_PATH
#define DEFAULT_DETECTION_PRECISION          PPR_FINE_PRECISION
#define DEFAULT_LANDMARK_DETECTOR_TYPE       PPR_NO_LANDMARK_DETECTOR
#define DEFAULT_NUM_THREADS                  sdk_utils_get_num_cpus()
#define DEFAULT_SEARCH_PRUNING               0
#define DEFAULT_TRACKING_CUTOFF              0
#define DEFAULT_NUM_VISUAL_INSTANCES         0
#define DEFAULT_SHOT_BOUNDARY_THRESHOLD      0.0
#define DEFAULT_MIN_SIZE                     PPR_MIN_MIN_SIZE
#define DEFAULT_MAX_SIZE                     PPR_MAX_MAX_SIZE
#define DEFAULT_NUM_FRAMES_BETWEEN_EXTRACTS  0

/*****************************************************************************
 * Local Function Prototypes
 *****************************************************************************/

static void print_usage_and_exit(const char *exe_name);

/*******************************************************************************
 * Functions
 ******************************************************************************/

/*
 * Sets parameters based on command-line arguments.  Uses defaults for any
 * parameters that are not specified.
 */
int parse_command_line(int argc, char *argv[], parameters_struct *params)
{
  int rv;
  int i_first_file;

  char *models_path_str = NULL;
  char *detection_precision_str = NULL;
  char *landmark_detector_type_str = NULL;

  int no_shot_boundary_detection = 0;
  int no_frame_numbers = 0;
  int no_print = 0;
  int no_binary_output = 0;
  int help = 0;

  /* Set default parameters */
  strcpy(params->models_path, DEFAULT_MODELS_PATH);
  params->detection_precision = DEFAULT_DETECTION_PRECISION;
  params->landmark_detector_type = DEFAULT_LANDMARK_DETECTOR_TYPE;
  params->num_threads = DEFAULT_NUM_THREADS;
  params->search_pruning = DEFAULT_SEARCH_PRUNING;
  params->tracking_cutoff = DEFAULT_TRACKING_CUTOFF;
  params->num_visual_instances = DEFAULT_NUM_VISUAL_INSTANCES;
  params->use_shot_boundary_detection = 1;
  params->shot_boundary_threshold = DEFAULT_SHOT_BOUNDARY_THRESHOLD;
  params->min_size = DEFAULT_MIN_SIZE;
  params->max_size = DEFAULT_MAX_SIZE;
  params->num_frames_between_extracts = DEFAULT_NUM_FRAMES_BETWEEN_EXTRACTS;
  params->overlays = 0;
  params->frame_numbers = 1;
  params->print = 1;
  params->binary_output = 1;
  
  /* Parse command line */
  i_first_file = sdk_cmd_line_utils_parse(argc, argv,
                                          "-models %S", &models_path_str,
                                          "-precision %S", &detection_precision_str,
                                          "-landmarks %S", &landmark_detector_type_str,
                                          "-threads %d", &params->num_threads,
                                          "-search_pruning %d", &params->search_pruning,
                                          "-tracking_cutoff %d", &params->tracking_cutoff,
                                          "-n_visual_instances %d", &params->num_visual_instances,
                                          "-no_shot_boundary_detection", &no_shot_boundary_detection,
                                          "-shot_boundary_threshold %f", &params->shot_boundary_threshold,
                                          "-min_size %d", &params->min_size,
                                          "-max_size %d", &params->max_size,
                                          "-n_frames_between_extracts %d", &params->num_frames_between_extracts,
                                          "-overlays", &params->overlays,
                                          "-no_frame_numbers", &no_frame_numbers,
                                          "-no_print", &no_print,
                                          "-no_binary_output", &no_binary_output,
                                          "-help", &help,
                                          (void*)0);
  if (i_first_file < 0)
  {
    fprintf(stderr, "ERROR: %s\n", sdk_cmd_line_utils_error_message(i_first_file));
    print_usage_and_exit(argv[0]);
  }

  if (help)
    print_usage_and_exit(argv[0]);

  if (argc <= i_first_file)
  {
    fprintf(stderr, "ERROR: input file(s) not specified\n");
    print_usage_and_exit(argv[0]);
  }

  if (params->num_frames_between_extracts != 0 && params->overlays)
  {
    fprintf(stderr, "ERROR: cannot overlay results if extracting frames periodically\n");
    fprintf(stderr, "       set -n_frames_between_extracts to 0 to do overlays\n");
    exit(EXIT_FAILURE);
  }

  /* Set models path */
  if (models_path_str != NULL)
  {
    strcpy(params->models_path, models_path_str);
    free(models_path_str);
  }

  /* Set detection precision */
  if (detection_precision_str != NULL)
  {
    rv = sdk_utils_get_precision_from_string(detection_precision_str, &params->detection_precision);
    free(detection_precision_str);
    if (rv < 0)
    {
      fprintf(stderr, "ERROR: invalid precision argument\n");
      print_usage_and_exit(argv[0]);
    }
  }

  /* Set landmark detector type */
  if (landmark_detector_type_str != NULL)
  {
    rv = sdk_utils_get_landmark_detector_from_string(landmark_detector_type_str, &params->landmark_detector_type);
    free(landmark_detector_type_str);
    if (rv < 0)
    {
      fprintf(stderr, "ERROR: invalid landmarks argument\n");
      print_usage_and_exit(argv[0]);
    }
  }

  /* Set option flags */
  if (no_shot_boundary_detection)
    params->use_shot_boundary_detection = 0;

  if (no_frame_numbers)
    params->frame_numbers = 0;
  
  if (no_print)
    params->print = 0;

  if (no_binary_output)
    params->binary_output = 0;

  return i_first_file;
}

/*******************************************************************************
 * Local Functions
 ******************************************************************************/

/*
 * Prints the program's usage message and then exits.
 */
static void print_usage_and_exit(const char *exe_name)
{
  fprintf(stderr, "\n");
  fprintf(stderr, "usage: %s [flags] <video | image list> ...\n", exe_name);
  fprintf(stderr, "\n");
  fprintf(stderr, "  where flags are one or more of the following:\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "   -models <string>                  :  path to models directory (default = %s)\n", 
          DEFAULT_MODELS_PATH);
  fprintf(stderr, "   -precision <string>               :  detection precision (coarse | fine) (default = %s)\n",
          sdk_utils_get_precision_string(DEFAULT_DETECTION_PRECISION));
  fprintf(stderr, "   -landmarks <string>               :  landmark detector (none | frontal | multi-pose | dual_frontal | dual_multi-pose) (default = %s)\n",
          sdk_utils_get_landmark_detector_string(DEFAULT_LANDMARK_DETECTOR_TYPE));
  fprintf(stderr, "   -threads <int>                    :  number of threads to use (default = number of CPU cores = %d)\n", 
          DEFAULT_NUM_THREADS);
  fprintf(stderr, "   -search_pruning <int>             :  search pruning aggressiveness (0 - %d) (default = %d)\n", 
          PPR_MAX_SEARCH_PRUNING_AGGRESSIVENESS, DEFAULT_SEARCH_PRUNING);
  fprintf(stderr, "   -tracking_cutoff <int>            :  tracking cutoff integer (%d - %d) (default = %d)\n",
          PPR_TRACKING_CUTOFF_MIN, PPR_TRACKING_CUTOFF_MAX, DEFAULT_TRACKING_CUTOFF);
  fprintf(stderr, "   -n_visual_instances <int>         :  number of visual instances per track (0 - %d) (default = %d)\n", 
          PPR_MAX_NUM_VISUAL_INSTANCES, DEFAULT_NUM_VISUAL_INSTANCES);
  fprintf(stderr, "   -no_shot_boundary_detection       :  disable shot boundary detection\n");
  fprintf(stderr, "   -shot_boundary_threshold <float>  :  shot boundary threshold (default = %.3f)\n", 
          DEFAULT_SHOT_BOUNDARY_THRESHOLD);
  fprintf(stderr, "   -min_size <int>                   :  minimum size (%d -%d) (default = %d)\n", 
          PPR_MIN_MIN_SIZE, PPR_MAX_MAX_SIZE, DEFAULT_MIN_SIZE);
  fprintf(stderr, "   -max_size <int>                   :  maximum size (%d -%d) (default = %d)\n", 
          PPR_MIN_MIN_SIZE, PPR_MAX_MAX_SIZE, DEFAULT_MAX_SIZE);
  fprintf(stderr, "   -n_frames_between_extracts <int>  :  number of frames between extracting results (0 means only extract at end) (default = %d)\n", 
          DEFAULT_NUM_FRAMES_BETWEEN_EXTRACTS);
  fprintf(stderr, "   -overlays                         :  write overlay images or video to disk (cannot use when -n_frames_between_extracts != 0)\n");
  fprintf(stderr, "   -no_frame_numbers                 :  suppress overlaying frame numbers on output\n");
  fprintf(stderr, "   -no_print                         :  suppress printing of detected objects to screen\n");
  fprintf(stderr, "   -no_binary_output                 :  suppress writing completed track list to binary file\n");
  fprintf(stderr, "   -help                             :  display this usage message and exit\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "example: %s -overlays ../sample_data/videos/video_01.mp4\n", exe_name);
  
  exit(EXIT_FAILURE);
}

/******************************************************************************/
