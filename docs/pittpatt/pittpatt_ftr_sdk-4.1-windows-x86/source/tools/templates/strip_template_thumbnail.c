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
 * TOOL : Strip Template Thumbnail
 *
 * Given a template file, this program removes the template's thumbnail and
 * overwrites the file.
 */

#include <stdio.h>
#include <stdlib.h>

#include "pittpatt_ftr_sdk.h"
#include "sdk_utils.h"

/******************************************************************************/

int main(int argc, char *argv[])
{
  ppr_context_type context;
  ppr_template_type template;
  
  if (argc != 2)
  {
    fprintf(stderr, "usage: %s <template>\n", argv[0]);
    return EXIT_FAILURE;
  }
  
  context = ppr_get_context();
  TRY(ppr_enable_recognition(context));
  
  TRY(ppr_disable_template_thumbnails(context));

  printf("Reading template %s without thumbnail...\n", argv[1]);  
  TRY(ppr_read_template(context, argv[1], &template));

  printf("Overwriting template %s...\n", argv[1]);
  TRY(ppr_write_template(context, argv[1], template));

  ppr_free_template(template);

  ppr_release_context(context);
  ppr_finalize_sdk();
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
