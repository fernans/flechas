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
 * TOOL : Get Dispersed Tracks From Completed Tracks
 *
 * Given a completed track list file this program gets a dispersed frontal
 * tracked object list for each completed track and writes them to file.
 */

#include <stdio.h>
#include <stdlib.h>

#include "pittpatt_ftr_sdk.h"
#include "sdk_utils.h"

/******************************************************************************/

int main(int argc, char *argv[])
{
  int i;
  ppr_context_type context;
  int max_num_objects;
  ppr_completed_track_list_type completed_track_list;
  char source_file_name[PPR_MAX_SOURCE_FILE_NAME_SIZE];
  ppr_tracked_object_list_type frontal_objects_list;
  char title[FILENAME_MAX];
  char out_file_name[FILENAME_MAX];
  
  if (argc != 3)
  {
    fprintf(stderr, "usage: %s <completed track list> <max num objects>\n", argv[0]);
    return EXIT_FAILURE;
  }
  max_num_objects = atoi(argv[2]);
  
  context = ppr_get_context();
  TRY(ppr_enable_tracking(context));

  printf("Reading completed track list %s...\n", argv[1]);  
  TRY(ppr_read_completed_track_list(context, argv[1], &completed_track_list, source_file_name));
  sdk_utils_get_file_title(source_file_name, title);

  for (i = 0; i < completed_track_list.num_completed_tracks; ++i)
  {
    printf("Getting dispersed frontal objects for completed track[%d] of %d...\n", i,
           completed_track_list.num_completed_tracks);
    TRY(ppr_get_dispersed_frontal_objects_from_track(context, completed_track_list.completed_tracks[i], max_num_objects,
                                                     &frontal_objects_list));
    sprintf(out_file_name, "%s_dispersed_track_%04d.bin", title, i);
    printf("Writing %s...\n", out_file_name);
    TRY(ppr_write_tracked_object_list(context, out_file_name, frontal_objects_list, source_file_name));
    ppr_free_tracked_object_list(frontal_objects_list);
  }

  ppr_free_completed_track_list(completed_track_list);

  ppr_release_context(context);
  ppr_finalize_sdk();
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
