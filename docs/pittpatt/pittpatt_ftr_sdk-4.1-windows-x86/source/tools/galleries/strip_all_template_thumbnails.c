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
 * TOOL : Strip All Template Thumbnails
 *
 * Given a gallery file, this program removes the thumbnail from every template
 * in the gallery and overwrites the file.
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
  
  if (argc != 2)
  {
    printf("usage: %s <gallery>\n", argv[0]);
    return EXIT_FAILURE;
  }

  context = ppr_get_context();
  TRY(ppr_enable_recognition(context));
  
  TRY(ppr_disable_template_thumbnails(context));

  printf("Reading gallery %s without thumbnails...\n", argv[1]);
  TRY(ppr_read_gallery(context, argv[1], &gallery));

  printf("Overwriting gallery %s...\n", argv[1]);
  TRY(ppr_write_gallery(context, argv[1], gallery));

  ppr_free_gallery(gallery);

  ppr_release_context(context);
  ppr_finalize_sdk();
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
