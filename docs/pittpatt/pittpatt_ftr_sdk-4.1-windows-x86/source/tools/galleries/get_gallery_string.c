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
 * TOOL : Get Gallery String
 *
 * Given a gallery file, this program prints the gallery's embedded string.
 */

#include <stdio.h>
#include <stdlib.h>

#include "pittpatt_ftr_sdk.h"
#include "sdk_utils.h"

/******************************************************************************/

int main(int argc, char *argv[])
{
  ppr_context_type context;
  ppr_gallery_type gallery;
  char gallery_string[PPR_MAX_GALLERY_STRING_SIZE];
  
  if (argc != 2)
  {
    fprintf(stderr, "usage: %s <gallery>\n", argv[0]);
    return EXIT_FAILURE;
  }
  
  context = ppr_get_context();
  TRY(ppr_enable_recognition(context));

  printf("Reading gallery %s...\n", argv[1]);  
  TRY(ppr_read_gallery(context, argv[1], &gallery));

  printf("Getting gallery string...\n");
  TRY(ppr_get_gallery_string(context, gallery, gallery_string));

  printf("%s\n", gallery_string);

  ppr_free_gallery(gallery);

  ppr_release_context(context);
  ppr_finalize_sdk();
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
