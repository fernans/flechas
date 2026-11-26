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
 * TOOL : Overlay Text On Image
 *
 * This program overlays text on an image at the specified position.
 */

#include <stdio.h>
#include <stdlib.h>

#include "pittpatt_raw_image_io.h"

#include "sdk_draw_utils.h"
#include "sdk_utils.h"

/*******************************************************************************
 * Constants
 ******************************************************************************/

#define DEFAULT_SCALE       2

/*******************************************************************************
 * Functions
 ******************************************************************************/

int main(int argc, const char *argv[])
{
  ppr_raw_image_type raw_image;
  sdk_draw_utils_point_type position;
  sdk_draw_utils_text_attributes_type text_attributes;
  char extension[FILENAME_MAX];
  char title[FILENAME_MAX];
  char output_file_name[FILENAME_MAX];

  if (argc < 5 || argc > 6)
  {
    fprintf(stderr, "Usage: %s <input image> <text> <x position> <y position> [scale = %d]\n", argv[0], DEFAULT_SCALE);
    return EXIT_FAILURE;
  }

  position.x = atoi(argv[3]);
  position.y = atoi(argv[4]);

  sdk_draw_utils_init_text_attributes(&text_attributes);
  text_attributes.color = SDK_DRAW_UTILS_WHITE;
  text_attributes.background_color = SDK_DRAW_UTILS_BLACK;
  if (argc == 6)
    text_attributes.scale = atoi(argv[5]);
  else
    text_attributes.scale = DEFAULT_SCALE;

  sdk_utils_get_file_title(argv[1], title);
  sdk_utils_get_file_extension(argv[1], extension);
  sprintf(output_file_name, "%s_with_text.%s", title, extension);
  
  ppr_raw_image_io_read(argv[1], &raw_image);
  ppr_raw_image_convert(&raw_image, PPR_RAW_IMAGE_RGB24);
  sdk_draw_utils_draw_string(&raw_image, argv[2], position, text_attributes);
  ppr_raw_image_io_write(output_file_name, raw_image);

  ppr_raw_image_free(raw_image);
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
