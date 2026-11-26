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
 * TOOL : Add Templates To Gallery
 *
 * Given a gallery file, this program adds the specified templates to it.
 */

#include <stdio.h>
#include <stdlib.h>

#include "pittpatt_ftr_sdk.h"
#include "sdk_utils.h"

/******************************************************************************/

int main(int argc, char *argv[])
{
  int i;
  ppr_context_type context;
  ppr_gallery_type gallery;
  ppr_template_type curr_template;
  int id;
  
  if (argc < 3)
  {
    fprintf(stderr, "usage: %s <gallery> <template> [...]\n", argv[0]);
    return EXIT_FAILURE;
  }
  
  context = ppr_get_context();
  TRY(ppr_enable_recognition(context));

  printf("Reading gallery %s...\n", argv[1]);  
  TRY(ppr_read_gallery(context, argv[1], &gallery));

  for (i = 2; i < argc; i++)
  {
    printf("Reading template %s and adding to gallery...\n", argv[i]);
    TRY(ppr_read_template(context, argv[i], &curr_template));
    TRY(ppr_copy_template_to_gallery(context, &gallery, curr_template, &id));
    printf("Added template ID = %d\n", id);
    ppr_free_template(curr_template);
  }
  
  printf("Writing gallery %s...\n", argv[1]);  
  TRY(ppr_write_gallery(context, argv[1], gallery));

  ppr_free_gallery(gallery);

  ppr_release_context(context);
  ppr_finalize_sdk();
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
