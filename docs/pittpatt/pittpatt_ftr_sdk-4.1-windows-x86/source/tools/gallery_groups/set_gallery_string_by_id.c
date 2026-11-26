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
 * TOOL : Set Gallery String By ID
 *
 * Given a gallery group file and a gallery ID, this program overwrites the
 * string in the specified gallery.
 */

#include <stdio.h>
#include <stdlib.h>

#include "pittpatt_ftr_sdk.h"
#include "sdk_utils.h"

/******************************************************************************/

int main(int argc, char *argv[])
{
  ppr_error_type err;
  int gallery_id;
  ppr_context_type context;
  ppr_gallery_group_type gallery_group;
  
  if (argc != 4)
  {
    fprintf(stderr, "usage: %s <gallery group> <gallery id> <string>\n", argv[0]);
    return EXIT_FAILURE;
  }

  gallery_id = atoi(argv[2]);
  
  context = ppr_get_context();
  TRY(ppr_enable_recognition(context));

  printf("Reading gallery group %s...\n", argv[1]);  
  TRY(ppr_read_gallery_group(context, argv[1], &gallery_group));

  printf("Setting string for gallery with ID = %d to %s...\n", gallery_id, argv[3]);
  
  err = ppr_set_gallery_string_by_id(context, &gallery_group, gallery_id, argv[3]);
  if (err == PPR_NO_GALLERY_WITH_MATCHING_ID)
  {
    fprintf(stderr, "Gallery group %s has no gallery with the ID %d\n", argv[1], gallery_id);
    return EXIT_FAILURE;
  }
  sdk_utils_check_ppr_error(err, "ppr_set_gallery_string_by_id");

  printf("Writing gallery group %s...\n", argv[1]);  
  TRY(ppr_write_gallery_group(context, argv[1], gallery_group));
  
  ppr_free_gallery_group(gallery_group);

  ppr_release_context(context);
  ppr_finalize_sdk();
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
