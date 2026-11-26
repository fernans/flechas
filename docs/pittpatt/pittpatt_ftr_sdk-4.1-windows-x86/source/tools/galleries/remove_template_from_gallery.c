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
 * TOOL : Remove Template From Gallery
 *
 * Given a gallery file, this program removes the template with the specified
 * ID.
 */

#include <stdio.h>
#include <stdlib.h>

#include "pittpatt_ftr_sdk.h"
#include "sdk_utils.h"

/******************************************************************************/

int main(int argc, char *argv[])
{
  ppr_error_type err;
  ppr_context_type context;
  ppr_gallery_type gallery;
  int id;
  
  if (argc != 3)
  {
    fprintf(stderr, "usage: %s <gallery> <template id>\n", argv[0]);
    return EXIT_FAILURE;
  }

  id = atoi(argv[2]);
  
  context = ppr_get_context();
  TRY(ppr_enable_recognition(context));

  printf("Reading gallery %s...\n", argv[1]);  
  TRY(ppr_read_gallery(context, argv[1], &gallery));

  printf("Removing template with ID %d...\n", id);
  err = ppr_remove_template_from_gallery(context, &gallery, id);
  if (err == PPR_NO_TEMPLATE_WITH_MATCHING_ID)
  {
    fprintf(stderr, "Gallery %s has no template with the ID %d\n", argv[1], id);
    return EXIT_FAILURE;
  }
    
  printf("Writing gallery %s...\n", argv[1]);  
  TRY(ppr_write_gallery(context, argv[1], gallery));

  ppr_free_gallery(gallery);

  ppr_release_context(context);
  ppr_finalize_sdk();
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
