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

#define DEFAULT_MODELS_PATH                      MODELS_PATH
#define DEFAULT_DETECTION_PRECISION              PPR_COARSE_PRECISION
#define DEFAULT_LANDMARK_DETECTOR_TYPE           PPR_FRONTAL_LANDMARK_DETECTOR
#define DEFAULT_NUM_THREADS                      sdk_utils_get_num_cpus()
#define DEFAULT_SEARCH_PRUNING                   0
#define DEFAULT_TRACKING_CUTOFF                  0
#define DEFAULT_NUM_VISUAL_INSTANCES             0
#define DEFAULT_SHOT_BOUNDARY_THRESHOLD          0.0f
#define DEFAULT_MIN_SIZE                         PPR_MIN_MIN_SIZE
#define DEFAULT_MAX_SIZE                         PPR_MAX_MAX_SIZE
#define DEFAULT_FRONTAL_YAW_CONSTRAINT           PPR_FRONTAL_YAW_CONSTRAINT_RESTRICTIVE
#define DEFAULT_EXTRACTION_TYPE                  PPR_EXTRACT_SINGLE
#define DEFAULT_AGGRESSIVENESS                   8
#define DEFAULT_MAX_DISPERSED_OBJECTS_PER_TRACK  50
#define DEFAULT_NUM_REPRESENTATIVE_TEMPLATES     5
#define DEFAULT_MATCH_THRESHOLD                  0.5f

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
  int i_first_file;

  char *models_path_str = NULL;
  char *detection_precision_str = NULL;
  char *landmark_detector_type_str = NULL;
  char *frontal_yaw_constraint_str = NULL;
  char *extraction_type_str = NULL;
  char *known_gallery_name_str = NULL;

  int no_shot_boundary_detection = 0;
  int no_frame_numbers = 0;
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
  params->frontal_yaw_constraint = DEFAULT_FRONTAL_YAW_CONSTRAINT;
  params->extraction_type = DEFAULT_EXTRACTION_TYPE;
  params->aggressiveness = DEFAULT_AGGRESSIVENESS;
  params->max_dispersed_objects_per_track = DEFAULT_MAX_DISPERSED_OBJECTS_PER_TRACK;
  params->num_representative_templates = DEFAULT_NUM_REPRESENTATIVE_TEMPLATES;
  params->use_known_gallery = 0;
  params->known_gallery_name[0] = '\0';
  params->match_threshold = DEFAULT_MATCH_THRESHOLD;
  params->frame_numbers = 1;
  
  /* Parse command line */
  i_first_file = sdk_cmd_line_utils_parse(argc, argv,
                                          "-models %S", &models_path_str,
                                          "-precision %S", &detection_precision_str,
                                          "-landmarks %S", &landmark_detector_type_str,
                                          "-threads %d", &params->num_threads,
                                          "-search_pruning%d", &params->search_pruning,
                                          "-tracking_cutoff %d", &params->tracking_cutoff,
                                          "-visual_attention", &params->use_visual_attention,
                                          "-no_shot_boundary_detection", &no_shot_boundary_detection,
                                          "-shot_boundary_threshold %f", &params->shot_boundary_threshold,
                                          "-min_size %d", &params->min_size,
                                          "-max_size %d", &params->max_size,
                                          "-yaw_constraint %S", &frontal_yaw_constraint_str,
                                          "-extract %S", &extraction_type_str,
                                          "-aggressiveness %d", &params->aggressiveness,
                                          "-max_dispersed_objects %d", &params->max_dispersed_objects_per_track,
                                          "-n_representative_templates %d", &params->num_representative_templates,
                                          "-known_gallery %S", &known_gallery_name_str,
                                          "-match_threshold %f", &params->match_threshold,
                                          "-no_frame_numbers", &no_frame_numbers,
                                          "-help", &help,
                                          (void*)0);
  if (i_first_file < 0)
  {
    fprintf(stderr, "ERROR: %s\n", sdk_cmd_line_utils_error_message(i_first_file));
    print_usage_and_exit(argv[0]);
  }

  if (i_first_file < 0 || help)
    print_usage_and_exit(argv[0]);

  if ((argc - i_first_file) != 2)
  {
    fprintf(stderr, "ERROR: one video and one results directory must be specified\n");
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

  /* Set template extraction type */
  if (extraction_type_str != NULL)
  {
    rv = sdk_utils_get_template_extraction_type_from_string(extraction_type_str, &params->extraction_type);
    free(extraction_type_str);
    if (rv < 0)
    {
      fprintf(stderr, "ERROR: invalid extraction type argument\n");
      print_usage_and_exit(argv[0]);
    }
  }

  /* Set known gallery name */
  if (known_gallery_name_str != NULL)
  {
    params->use_known_gallery = 1;
    strcpy(params->known_gallery_name, known_gallery_name_str);
    free(known_gallery_name_str);
  }
  
  /* Set option flags */
  if (no_shot_boundary_detection)
    params->use_shot_boundary_detection = 0;

  if (no_frame_numbers)
    params->frame_numbers = 0;

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
  fprintf(stderr, "usage: %s [flags] <video> <results directory>\n", exe_name);
  fprintf(stderr, "\n");
  fprintf(stderr, "  where flags are one or more of the following:\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "   -models <string>                   :  path to models directory (default = %s)\n", DEFAULT_MODELS_PATH);
  fprintf(stderr, "   -precision <string>                :  detection precision (coarse | fine) (default = %s)\n",
          sdk_utils_get_precision_string(DEFAULT_DETECTION_PRECISION));
  fprintf(stderr, "   -landmarks <string>                :  landmark detector (none | frontal | multi-pose | dual_frontal | dual_multi-pose) (default = %s)\n",
          sdk_utils_get_landmark_detector_string(DEFAULT_LANDMARK_DETECTOR_TYPE));
  fprintf(stderr, "   -threads <int>                     :  number of threads to use (default = number of CPU cores = %d)\n", DEFAULT_NUM_THREADS);
  fprintf(stderr, "   -search_pruning <int>              :  search pruning aggressiveness (0 - %d) (default = %d)\n", 
          PPR_MAX_SEARCH_PRUNING_AGGRESSIVENESS, DEFAULT_SEARCH_PRUNING);
  fprintf(stderr, "   -tracking_cutoff <int>             :  tracking cutoff integer (%d - %d) (default = %d)\n",
          PPR_TRACKING_CUTOFF_MIN, PPR_TRACKING_CUTOFF_MAX, DEFAULT_TRACKING_CUTOFF);
  fprintf(stderr, "   -visual_attention                  :  use visual attention\n");
  fprintf(stderr, "   -no_shot_boundary_detection        :  disable shot boundary detection\n");
  fprintf(stderr, "   -shot_boundary_threshold <float>   :  shot boundary threshold (default = %.3f)\n", 
          DEFAULT_SHOT_BOUNDARY_THRESHOLD);
  fprintf(stderr, "   -min_size <int>                    :  minimum size (%d - %d) (default = %d)\n", 
          PPR_MIN_MIN_SIZE, PPR_MAX_MAX_SIZE, DEFAULT_MIN_SIZE);
  fprintf(stderr, "   -max_size <int>                    :  maximum size (%d - %d) (default = %d)\n", 
          PPR_MIN_MIN_SIZE, PPR_MAX_MAX_SIZE, DEFAULT_MAX_SIZE);
  fprintf(stderr, "   -yaw_constraint <string>           :  yaw constraint for frontal faces (restrictive | permissive) (default = %s)\n", sdk_utils_get_frontal_yaw_constraint_string(DEFAULT_FRONTAL_YAW_CONSTRAINT));
  fprintf(stderr, "   -extract <string>                  :  type of template to extract (single | double) (default = %s)\n", sdk_utils_get_template_extraction_type_string(DEFAULT_EXTRACTION_TYPE));
  fprintf(stderr, "   -aggressiveness <int>              :  clustering aggressiveness (0 - %d) (default = %d)\n", PPR_MAX_CLUSTERING_AGGRESSIVENESS, DEFAULT_AGGRESSIVENESS);
  fprintf(stderr, "   -max_dispersed_objects <int>       :  maximum number of dispersed objects to select from each track (default = %d)\n", DEFAULT_MAX_DISPERSED_OBJECTS_PER_TRACK);
  fprintf(stderr, "   -n_representative_templates <int>  :  number of representative templates to select from each track (default = %d)\n", DEFAULT_NUM_REPRESENTATIVE_TEMPLATES);
  fprintf(stderr, "   -known_gallery <string>            :  compare subjects to specified known gallery\n");
  fprintf(stderr, "   -match_threshold <float>           :  threshold that must be exceeded to match a subject in the known gallery (default = %.3f)\n", DEFAULT_MATCH_THRESHOLD);
  fprintf(stderr, "   -no_frame_numbers                  :  suppress overlaying frame numbers on output\n");
  fprintf(stderr, "   -help                              :  display this usage message and exit\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "example: %s -known_gallery pittpatt_faces_gallery.bin ../sample_data/videos/pittpatt_01.mp4 pittpatt_01_results\n", exe_name);
  fprintf(stderr, "\n");
  fprintf(stderr, " (see recognition-create_subject_gallery to create pittpatt_faces_gallery.bin)\n");
  fprintf(stderr, "\n");
  
  exit(EXIT_FAILURE);
}

/*****************************************************************************/
