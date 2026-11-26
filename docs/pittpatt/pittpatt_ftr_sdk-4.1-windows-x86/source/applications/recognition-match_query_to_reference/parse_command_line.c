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

#define DEFAULT_MODELS_PATH             MODELS_PATH
#define DEFAULT_DETECTION_PRECISION     PPR_FINE_PRECISION
#define DEFAULT_LANDMARK_DETECTOR_TYPE  PPR_DUAL_FRONTAL_LANDMARK_DETECTOR
#define DEFAULT_NUM_THREADS             sdk_utils_get_num_cpus()
#define DEFAULT_SEARCH_PRUNING          0
#define DEFAULT_MIN_SIZE                PPR_MIN_MIN_SIZE
#define DEFAULT_MAX_SIZE                PPR_MAX_MAX_SIZE
#define DEFAULT_ADAPTIVE_MIN_SIZE       0.01f
#define DEFAULT_ADAPTIVE_MAX_SIZE       1.00f
#define DEFAULT_DETECTION_THRESHOLD     -1.0f
#define DEFAULT_FRONTAL_YAW_CONSTRAINT  PPR_FRONTAL_YAW_CONSTRAINT_RESTRICTIVE

/*******************************************************************************
 * Local Function Prototypes
 ******************************************************************************/

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
  int i_first_input_file;

  char *models_path_str = NULL;
  char *detection_precision_str = NULL;
  char *landmark_detector_type_str = NULL;
  char *frontal_yaw_constraint_str = NULL;

  int help;

  /* Set default parameters */
  strcpy(params->models_path, DEFAULT_MODELS_PATH);
  params->detection_precision = DEFAULT_DETECTION_PRECISION;
  params->landmark_detector_type = DEFAULT_LANDMARK_DETECTOR_TYPE;
  params->num_threads = DEFAULT_NUM_THREADS;
  params->search_pruning = DEFAULT_SEARCH_PRUNING;
  params->min_size = DEFAULT_MIN_SIZE;
  params->max_size = DEFAULT_MAX_SIZE;
  params->adaptive_min_size = DEFAULT_ADAPTIVE_MIN_SIZE;
  params->adaptive_max_size = DEFAULT_ADAPTIVE_MAX_SIZE;
  params->detection_threshold = DEFAULT_DETECTION_THRESHOLD;
  params->frontal_yaw_constraint = DEFAULT_FRONTAL_YAW_CONSTRAINT;
  
  /* Parse command line */
  i_first_input_file = sdk_cmd_line_utils_parse(argc, argv,
                                                "-models %S", &models_path_str,
                                                "-precision %S", &detection_precision_str,
                                                "-landmarks %S", &landmark_detector_type_str,
                                                "-threads %d", &params->num_threads,
                                                "-search_pruning %d", &params->search_pruning,
                                                "-min_size %d", &params->min_size,
                                                "-max_size %d", &params->max_size,
                                                "-adaptive_min %f", &params->adaptive_min_size,
                                                "-adaptive_max %f", &params->adaptive_max_size,
                                                "-detection_threshold %f", &params->detection_threshold,
                                                "-yaw_constraint %S", &frontal_yaw_constraint_str,
                                                "-help", &help,
                                                (void*)0);
  if (i_first_input_file < 0)
  {
    fprintf(stderr, "ERROR: %s\n", sdk_cmd_line_utils_error_message(i_first_input_file));
    print_usage_and_exit(argv[0]);
  }

  if (help)
    print_usage_and_exit(argv[0]);

  if ((argc - i_first_input_file) < 2)
  {
    fprintf(stderr, "ERROR: input file(s) not specified\n");
    print_usage_and_exit(argv[0]);
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
    if (params->landmark_detector_type == PPR_NO_LANDMARK_DETECTOR)
    {
      fprintf(stderr, "ERROR: landmarks required\n");
      print_usage_and_exit(argv[0]);
    }
  }

  /* Set frontal yaw constraint */
  if (frontal_yaw_constraint_str != NULL)
  {
    rv = sdk_utils_get_frontal_yaw_constraint_from_string(frontal_yaw_constraint_str, &params->frontal_yaw_constraint);
    free(frontal_yaw_constraint_str);
    if (rv < 0)
    {
      fprintf(stderr, "ERROR: invalid yaw constraint argument\n");
      print_usage_and_exit(argv[0]);
    }
  }

  return i_first_input_file;
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
  fprintf(stderr, "usage: %s [flags] <reference image> <query image name | query image list> ...\n", exe_name);
  fprintf(stderr, "\n");
  fprintf(stderr, "  where flags are one or more of the following:\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "   -models <string>              :  path to models directory (default = %s)\n", DEFAULT_MODELS_PATH);
  fprintf(stderr, "   -precision <string>           :  detection precision (coarse | fine) (default = %s)\n",
          sdk_utils_get_precision_string(DEFAULT_DETECTION_PRECISION));
  fprintf(stderr, "   -landmarks <string>           :  landmark detector (frontal | multi-pose | dual_frontal | dual_multi-pose) (default = %s)\n",
          sdk_utils_get_landmark_detector_string(DEFAULT_LANDMARK_DETECTOR_TYPE));
  fprintf(stderr, "   -threads <int>                :  number of threads to use (default = number of CPU cores = %d)\n", 
          DEFAULT_NUM_THREADS);
  fprintf(stderr, "   -search_pruning <int>         :  search pruning aggressiveness (0 - %d) (default = %d)\n", 
          PPR_MAX_SEARCH_PRUNING_AGGRESSIVENESS, DEFAULT_SEARCH_PRUNING);
  fprintf(stderr, "   -min_size <int>               :  minimum size (%d - %d) (default = %d)\n", 
          PPR_MIN_MIN_SIZE, PPR_MAX_MAX_SIZE, DEFAULT_MIN_SIZE);
  fprintf(stderr, "   -max_size <int>               :  maximum size (%d - %d) (default = %d)\n", 
          PPR_MIN_MIN_SIZE, PPR_MAX_MAX_SIZE, DEFAULT_MAX_SIZE);
  fprintf(stderr, "   -adaptive_min <float>         :  adaptive minimum size (0.0 - 1.0) (default = %.2f)\n", 
          DEFAULT_ADAPTIVE_MIN_SIZE);
  fprintf(stderr, "   -adaptive_max <float>         :  adaptive maximum size (0.0 - 1.0) (default = %.2f)\n", 
          DEFAULT_ADAPTIVE_MAX_SIZE);
  fprintf(stderr, "   -detection_threshold <float>  :  detection threshold (-1.0 - 1.0) (default = %.3f)\n", 
          DEFAULT_DETECTION_THRESHOLD);
  fprintf(stderr, "   -yaw_constraint <string>      :  yaw constraint for frontal faces (restrictive | permissive) (default = %s)\n",
          sdk_utils_get_frontal_yaw_constraint_string(DEFAULT_FRONTAL_YAW_CONSTRAINT));
  fprintf(stderr, "   -help                         :  display this usage message and exit\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "example: %s ../sample_data/images/still_images/image_043.jpg image_lists/still_images_all.txt\n", exe_name);

  exit(EXIT_FAILURE);
}

/*****************************************************************************/
