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


#ifndef PARSE_COMMAND_LINE_H
#define PARSE_COMMAND_LINE_H

#include "pittpatt_ftr_sdk.h"

/* If C++ then we need to extern "C". Compiler defines __cplusplus */
#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Data Types
 ******************************************************************************/
  
typedef struct
{
  char models_path[FILENAME_MAX];
  ppr_precision_type detection_precision;
  ppr_landmark_detector_type landmark_detector_type;
  int num_threads;
  int search_pruning;
  int tracking_cutoff;
  int use_visual_attention;
  int num_visual_instances;
  int use_shot_boundary_detection;
  float shot_boundary_threshold;
  int min_size;
  int max_size;
  ppr_frontal_yaw_constraint_type frontal_yaw_constraint;
  ppr_template_extraction_type extraction_type;
  int aggressiveness;
  int max_dispersed_objects_per_track;
  int num_representative_templates;
  int use_known_gallery;
  char known_gallery_name[FILENAME_MAX];
  float match_threshold;
  int frame_numbers;
} parameters_struct;

/*******************************************************************************
 * Function Prototypes
 ******************************************************************************/

int parse_command_line(int argc, char *argv[], parameters_struct *params);
  
/******************************************************************************/
  
#ifdef __cplusplus
}
#endif

#endif /* PARSE_COMMAND_LINE_H */
