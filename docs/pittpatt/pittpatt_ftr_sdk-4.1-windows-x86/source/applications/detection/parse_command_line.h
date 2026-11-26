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

/*****************************************************************************
 * Data Types
 *****************************************************************************/

typedef struct
{
  char models_path[FILENAME_MAX];
  ppr_precision_type detection_precision;
  ppr_landmark_detector_type landmark_detector_type;
  int num_threads;
  int search_pruning;
  int min_size;
  int max_size;
  float adaptive_min_size;
  float adaptive_max_size;
  float detection_threshold;
  int x1, y1, x2, y2;

  int best_frontal;
  int overlays;
  int thumbnails;
  int print;
  int binary_output;
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
