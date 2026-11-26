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
 * TOOL : Overlay Completed Track List
 *
 * Given a completed track list file this program overlays polygons onto the
 * input source.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pittpatt_ftr_sdk.h"
#include "pittpatt_video_io.h"
#include "sdk_utils.h"

/*******************************************************************************
 * Constants
 ******************************************************************************/

#define MIN_OVERLAY_CONFIDENCE 2
#define OVERLAY_FRAME_NUMBERS  1
#define OUTPUT_IMAGE_FORMAT    "png"
#define OUTPUT_VIDEO_FORMAT    "mp4"

/*******************************************************************************
 * Functions
 ******************************************************************************/

int main(int argc, char *argv[])
{
  ppr_context_type context;
  ppr_completed_track_list_type completed_track_list;
  char source_file_name[PPR_MAX_SOURCE_FILE_NAME_SIZE];
  char extension[FILENAME_MAX];
  int source_is_video;
  sdk_utils_file_list_type image_list;
  
  if (argc != 2)
  {
    fprintf(stderr, "usage: %s <completed track list>\n", argv[0]);
    return EXIT_FAILURE;
  }
  
  context = ppr_get_context();
  TRY(ppr_enable_tracking(context));

  printf("Reading completed track list %s...\n", argv[1]);  
  TRY(ppr_read_completed_track_list(context, argv[1], &completed_track_list, source_file_name));

  sdk_utils_get_file_extension(source_file_name, extension);
  if (strcmp(extension, "txt") == 0)
    source_is_video = 0;
  else
    source_is_video = 1;

  printf("Overlaying completed track list on %s...\n", source_file_name);
  if (source_is_video)
  {
    sdk_utils_overlay_completed_track_list_on_video(source_file_name, completed_track_list, MIN_OVERLAY_CONFIDENCE,
                                                    OVERLAY_FRAME_NUMBERS, OUTPUT_VIDEO_FORMAT);
  }
  else
  {
    sdk_utils_read_file_list(source_file_name, &image_list); 
    sdk_utils_overlay_completed_track_list_on_image_list(image_list, completed_track_list, MIN_OVERLAY_CONFIDENCE,
                                                         OVERLAY_FRAME_NUMBERS, OUTPUT_IMAGE_FORMAT);
    sdk_utils_free_file_list(image_list);
  }

  printf("Complete\n");
  
  ppr_free_completed_track_list(completed_track_list);

  ppr_release_context(context);
  ppr_finalize_sdk();
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
