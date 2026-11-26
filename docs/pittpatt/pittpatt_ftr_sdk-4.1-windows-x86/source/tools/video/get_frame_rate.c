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
 * TOOL : Get Frame Rate
 *
 * This program prints the frame rate of a video file.
 */

#include <stdio.h>
#include <stdlib.h>

#include "pittpatt_ftr_sdk.h"
#include "pittpatt_video_io.h"
#include "sdk_utils.h"

/******************************************************************************/

int main(int argc, char *argv[])
{
  ppr_video_io_type video;
  int frame_rate_num, frame_rate_denom;
  float frame_rate;
  
  if (argc != 2)
  {
    fprintf(stderr, "usage: %s <video>\n", argv[0]);
    return EXIT_FAILURE;
  }

  printf("Opening video %s...\n", argv[1]);
  TRY_VIDEO(ppr_video_io_open(&video, argv[1]));

  TRY_VIDEO(ppr_video_io_get_frame_rate(video, &frame_rate_num, &frame_rate_denom));
  frame_rate = (float) frame_rate_num / (float) frame_rate_denom;
  printf("Frame rate: (%d/%d) = %.3f\n", frame_rate_num, frame_rate_denom, frame_rate);

  TRY_VIDEO(ppr_video_io_close(video));
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
