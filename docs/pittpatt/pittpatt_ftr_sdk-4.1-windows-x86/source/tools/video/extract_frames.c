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
 * TOOL : Extract frames
 *
 * This program extracts a range of frames from a video as images.  If the stop
 * frame is beyond the end of the video, frames will be extracted through the
 * end of the video.
 */

#include <stdio.h>
#include <stdlib.h>

#include "pittpatt_ftr_sdk.h"
#include "pittpatt_raw_image_io.h"
#include "pittpatt_video_io.h"
#include "sdk_utils.h"

/*******************************************************************************
 * Constants
 ******************************************************************************/

#define OUTPUT_IMAGE_FORMAT "jpg"

/*******************************************************************************
 * Functions
 ******************************************************************************/

int main(int argc, char *argv[])
{
  ppr_video_io_type video;
  int start_frame;
  int stop_frame;
  int frame_num;
  char title[FILENAME_MAX];
  ppr_raw_image_type raw_image;
  char image_name[FILENAME_MAX];
  
  if (argc != 4)
  {
    fprintf(stderr, "usage: %s <video> <start frame> <stop frame>\n", argv[0]);
    fprintf(stderr, "  where frame numbering starts at 0\n");
    return EXIT_FAILURE;
  }

  start_frame = atoi(argv[2]);
  stop_frame = atoi(argv[3]);
  if (start_frame < 0 || stop_frame < start_frame)
  {
    fprintf(stderr, "ERROR: illegal frame range [%d, %d]\n", start_frame, stop_frame);
    return EXIT_FAILURE;
  }

  sdk_utils_get_file_title(argv[1], title);

  printf("Opening video %s...\n", argv[1]);
  TRY_VIDEO(ppr_video_io_open(&video, argv[1]));

  frame_num = 0;
  while (ppr_video_io_is_frame_available(video) && frame_num <= stop_frame)
  {
    printf("Processing frame[%d]\n", frame_num);
    
    if (frame_num >= start_frame)
    {
      TRY_VIDEO(ppr_video_io_get_frame(video, &raw_image));
      sprintf(image_name, "%s_frame_%05d.%s", title, frame_num, OUTPUT_IMAGE_FORMAT);
      printf("Writing %s...\n", image_name);
      TRY_RAW_IMAGE(ppr_raw_image_io_write(image_name, raw_image));
      ppr_raw_image_free(raw_image);
    }

    TRY_VIDEO(ppr_video_io_step_forward(video));
    frame_num++;
  }

  TRY_VIDEO(ppr_video_io_close(video));
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
