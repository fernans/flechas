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
 * TOOL : Print Completed Track List Summaries
 *
 * Given a completed track list file this program prints the track summary
 * information for each track in a readable format.
 */

#include <stdio.h>
#include <stdlib.h>

#include "pittpatt_ftr_sdk.h"
#include "sdk_utils.h"

/******************************************************************************/

int main(int argc, char *argv[])
{
  ppr_context_type context;
  ppr_completed_track_list_type completed_track_list;
  char source_file_name[PPR_MAX_SOURCE_FILE_NAME_SIZE];
  
  if (argc != 2)
  {
    fprintf(stderr, "usage: %s <completed track list>\n", argv[0]);
    return EXIT_FAILURE;
  }
  
  context = ppr_get_context();
  TRY(ppr_enable_tracking(context));

  printf("Reading completed track list %s...\n", argv[1]);  
  TRY(ppr_read_completed_track_list(context, argv[1], &completed_track_list, source_file_name));

  printf("Source File Name: %s\n", source_file_name);
  sdk_utils_print_completed_track_list(stdout, completed_track_list, 0, 0, 1);

  ppr_free_completed_track_list(completed_track_list);

  ppr_release_context(context);
  ppr_finalize_sdk();
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
