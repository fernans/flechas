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
 * TOOL : Version
 *
 * This program prints the SDK version information,
 */

#include <stdio.h>
#include <stdlib.h>

#include "pittpatt_ftr_sdk.h"

/******************************************************************************/

int main(int argc, char *argv[])
{
  printf("%s\n", ppr_version());
  return EXIT_SUCCESS;
}

/******************************************************************************/
