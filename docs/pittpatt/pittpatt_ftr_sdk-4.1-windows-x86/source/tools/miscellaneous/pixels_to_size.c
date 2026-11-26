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
 * TOOL : Pixels To Size
 *
 * This program takes the number of pixels between the eyes and returns the size.
 */

#include <stdio.h>
#include <stdlib.h>

#include "pittpatt_ftr_sdk.h"
#include "sdk_utils.h"

/******************************************************************************/

int main(int argc, char *argv[])
{
  int pixels;
  float size;

  if (argc != 2)
  {
    printf("usage: %s <pixels between eyes>\n", argv[0]);
    return EXIT_FAILURE;
  }

  pixels = atoi(argv[1]);

  TRY(ppr_get_size_from_pixels(pixels, &size));

  printf("%d pixels between the eyes = size of %.3f\n", pixels, size);

  return EXIT_SUCCESS;
}

/******************************************************************************/
