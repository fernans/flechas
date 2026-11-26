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
 * TOOL : Convert Image
 *
 * This program converts an image to a different type.
 */

#include <stdio.h>
#include <stdlib.h>

#include "pittpatt_ftr_sdk.h"
#include "pittpatt_raw_image_io.h"

#include "sdk_utils.h"

/******************************************************************************/

int main(int argc, char *argv[])
{
  ppr_raw_image_type raw_image;
  int quality = 85;
  
  if (argc < 3 || argc > 4)
  {
    fprintf(stderr, "usage: %s <input image> <output image> [quality=85]\n", argv[0]);
    return EXIT_FAILURE;
  }

  if (argc == 4)
    quality = atoi(argv[3]);

  TRY_RAW_IMAGE(ppr_raw_image_io_read(argv[1], &raw_image));
  TRY_RAW_IMAGE(ppr_raw_image_io_write_with_quality(argv[2], raw_image, quality));
  ppr_raw_image_free(raw_image);
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
