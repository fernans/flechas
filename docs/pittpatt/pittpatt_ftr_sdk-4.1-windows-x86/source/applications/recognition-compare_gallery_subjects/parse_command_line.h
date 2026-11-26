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
  int num_threads;
  int max_ranked_list_length;
  float ranked_list_threshold;
  int unique_matches;
  int use_transitive_matching;

  int print_relationships;
} parameters_struct;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

int parse_command_line(int argc, char *argv[], parameters_struct *params);

/******************************************************************************/
  
#ifdef __cplusplus
}
#endif

#endif /* PARSE_COMMAND_LINE_H */
