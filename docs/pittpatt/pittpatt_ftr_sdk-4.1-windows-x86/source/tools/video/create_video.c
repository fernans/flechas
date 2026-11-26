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
 * TOOL : Create Video
 *
 * This program creates a video from an image list.
 */

#include <stdio.h>
#include <stdlib.h>

#include "pittpatt_ftr_sdk.h"
#include "pittpatt_raw_image.h"
#include "pittpatt_raw_image_io.h"
#include "pittpatt_video_io.h"
#include "sdk_utils.h"

/*******************************************************************************
 * Constants
 ******************************************************************************/

#define OUTPUT_VIDEO_FORMAT  "mp4"
#define DEFAULT_FRAME_RATE   30
#define DEFAULT_BIT_RATE     1000

/*******************************************************************************
 * Functions
 ******************************************************************************/

int main(int argc, char *argv[])
{
  char title[FILENAME_MAX];
  char output_file_name[FILENAME_MAX];
  int bit_rate = DEFAULT_BIT_RATE;
  int frame_rate = DEFAULT_FRAME_RATE;
  sdk_utils_file_list_type image_list;
  ppr_raw_image_type image;
  ppr_video_io_type video;
  int i;

  if (argc < 2 || argc > 4)
  {
    fprintf(stderr, "usage: %s <image list> [frame rate = %d] [bit rate kbps = %d]\n", argv[0], frame_rate, bit_rate);
    return EXIT_FAILURE;
  }

  if (argc >= 3)
    frame_rate = atoi(argv[2]);

  if (argc == 4)
    bit_rate = atoi(argv[3]);

  if (frame_rate <= 0)
  {
    fprintf(stderr, "ERROR: invalid frame rate (%d)\n", frame_rate);
    return EXIT_FAILURE;
  }

  if (bit_rate <= 0)
  {
    fprintf(stderr, "ERROR: invalid bit rate (%d)\n", bit_rate);
    return EXIT_FAILURE;
  }

  sdk_utils_get_file_title(argv[1], title);
  sprintf(output_file_name, "%s.%s", title, OUTPUT_VIDEO_FORMAT);

  sdk_utils_read_file_list(argv[1], &image_list);

  TRY_VIDEO(ppr_video_io_create(&video, output_file_name, bit_rate, frame_rate, 1));

  for (i = 0; i < image_list.size; i++)
  {
    TRY_RAW_IMAGE(ppr_raw_image_io_read(image_list.filenames[i], &image));
    TRY_VIDEO(ppr_video_io_add_frame(video, image));
    ppr_raw_image_free(image);
    printf("\r%d / %d ", i+1, image_list.size);
  }
  
  printf("\n");

  TRY_VIDEO(ppr_video_io_close(video));
  sdk_utils_free_file_list(image_list);
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
