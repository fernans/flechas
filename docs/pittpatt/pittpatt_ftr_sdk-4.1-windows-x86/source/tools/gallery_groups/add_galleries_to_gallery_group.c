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
 * TOOL : Add Galleries To Gallery Group
 *
 * Given a gallery group file, this program adds the specified galleries to it.
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
  ppr_gallery_group_type gallery_group;
  ppr_gallery_type gallery;
  int gallery_id;
  
  if (argc < 3)
  {
    fprintf(stderr, "usage: %s <gallery group> <gallery> [...]\n", argv[0]);
    return EXIT_FAILURE;
  }
  
  context = ppr_get_context();
  TRY(ppr_enable_recognition(context));

  printf("Reading gallery group %s...\n", argv[1]);  
  TRY(ppr_read_gallery_group(context, argv[1], &gallery_group));

  for (i = 2; i < argc; i++)
  {
    printf("Reading gallery %s and adding to gallery group...\n", argv[i]);
    TRY(ppr_read_gallery(context, argv[i], &gallery));
    TRY(ppr_move_gallery_to_gallery_group(context, &gallery_group, &gallery, &gallery_id));
  }
  
  printf("Writing gallery group %s...\n", argv[1]);  
  TRY(ppr_write_gallery_group(context, argv[1], gallery_group));

  ppr_free_gallery_group(gallery_group);

  ppr_release_context(context);
  ppr_finalize_sdk();
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
