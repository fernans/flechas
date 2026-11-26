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
 * TOOL : Get Gallery By ID
 *
 * Given a gallery group file and a gallery id, this program writes a gallery
 * file.
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
  ppr_gallery_type gallery;
  char title[FILENAME_MAX];
  char gallery_file_name[FILENAME_MAX];
  
  if (argc != 3)
  {
    fprintf(stderr, "usage: %s <gallery group> <gallery id>\n", argv[0]);
    return EXIT_FAILURE;
  }

  gallery_id = atoi(argv[2]);
  
  context = ppr_get_context();
  TRY(ppr_enable_recognition(context));

  printf("Reading gallery group %s...\n", argv[1]);  
  TRY(ppr_read_gallery_group(context, argv[1], &gallery_group));
  
  printf("Getting gallery with ID = %d...\n", gallery_id);
  
  err = ppr_get_gallery_reference_by_id(context, gallery_group, gallery_id, &gallery);
  if (err == PPR_NO_GALLERY_WITH_MATCHING_ID)
  {
    fprintf(stderr, "Gallery group %s has no gallery with the ID %d\n", argv[1], gallery_id);
    return EXIT_FAILURE;
  }
  sdk_utils_check_ppr_error(err, "ppr_get_gallery_reference_by_id");

  sdk_utils_get_file_title(argv[1], title);
  sprintf(gallery_file_name, "%s_gallery_%04d.bin", title, gallery_id);
  printf("Writing gallery %s...\n", gallery_file_name);
  
  TRY(ppr_write_gallery(context, gallery_file_name, gallery));
  
  TRY(ppr_release_gallery_reference(context, gallery_group, &gallery));

  ppr_free_gallery_group(gallery_group);

  ppr_release_context(context);
  ppr_finalize_sdk();
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
