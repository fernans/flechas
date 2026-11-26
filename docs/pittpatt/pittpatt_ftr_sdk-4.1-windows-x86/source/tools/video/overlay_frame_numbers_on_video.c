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
 * TOOL : Clip Video
 *
 * This program clips a video, creating a new video that contains the specified
 * range of frames.  If the stop frame is beyond the end of the video, all
 * frames through the end of the video will be used.
 */

#include <stdio.h>
#include <stdlib.h>

#include "pittpatt_ftr_sdk.h"
#include "pittpatt_video_io.h"
#include "sdk_utils.h"

/*******************************************************************************
 * Constants
 ******************************************************************************/

#define OUTPUT_VIDEO_FORMAT  "mp4"
#define DEFAULT_BIT_RATE     1000

/*******************************************************************************
 * Functions
 ******************************************************************************/

int main(int argc, char *argv[])
{
  ppr_video_io_type input_video;
  char title[FILENAME_MAX];
  int frame_rate_numerator;
  int frame_rate_denominator;
  int bit_rate = DEFAULT_BIT_RATE;
  ppr_video_io_type output_video;
  char output_file_name[FILENAME_MAX];
  int frame_num;
  ppr_raw_image_type raw_image;
  
  if (argc < 2 || argc > 3)
  {
    fprintf(stderr, "usage: %s <video> [bit rate kbps = %d]\n", argv[0], bit_rate);
    return EXIT_FAILURE;
  }

  if (argc == 3)
    bit_rate = atoi(argv[2]);

  sdk_utils_get_file_title(argv[1], title);

  printf("Opening input video %s...\n", argv[1]);
  TRY_VIDEO(ppr_video_io_open(&input_video, argv[1]));
  TRY_VIDEO(ppr_video_io_get_frame_rate(input_video, &frame_rate_numerator, &frame_rate_denominator));

  sprintf(output_file_name, "%s_with_frame_numbers.%s", title, OUTPUT_VIDEO_FORMAT);
  printf("Creating output video %s...\n", output_file_name);
  TRY_VIDEO(ppr_video_io_create(&output_video, output_file_name, bit_rate,
                                frame_rate_numerator, frame_rate_denominator));
  
  frame_num = 0;
  while (ppr_video_io_is_frame_available(input_video))
  {
    printf("Processing frame[%d]\n", frame_num);
    
    TRY_VIDEO(ppr_video_io_get_frame(input_video, &raw_image));
    sdk_utils_overlay_frame_number(&raw_image, frame_num);
    TRY_VIDEO(ppr_video_io_add_frame(output_video, raw_image));
    ppr_raw_image_free(raw_image);

    TRY_VIDEO(ppr_video_io_step_forward(input_video));
    frame_num++;
  }

  TRY_VIDEO(ppr_video_io_close(input_video));
  TRY_VIDEO(ppr_video_io_close(output_video));
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
