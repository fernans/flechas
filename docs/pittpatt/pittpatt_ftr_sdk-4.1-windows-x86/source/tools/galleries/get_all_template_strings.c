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
 * TOOL : Get All Template Strings
 *
 * Given a gallery file, this program prints the embedded string for every
 * template in the gallery.
 */

#include <stdio.h>
#include <stdlib.h>

#include "pittpatt_ftr_sdk.h"
#include "sdk_utils.h"

/******************************************************************************/

int main(int argc, char *argv[])
{
  int t;
  ppr_context_type context;
  ppr_gallery_type gallery;
  ppr_template_id_list_type template_id_list;
  char template_string[PPR_MAX_TEMPLATE_STRING_SIZE];
  
  if (argc != 2)
  {
    printf("usage: %s <gallery>\n", argv[0]);
    return EXIT_FAILURE;
  }

  context = ppr_get_context();
  TRY(ppr_enable_recognition(context));

  printf("Reading gallery %s...\n", argv[1]);
  TRY(ppr_read_gallery(context, argv[1], &gallery));

  printf("Getting template id list and displaying string for each template...\n");
  TRY(ppr_get_template_id_list_from_gallery(context, gallery, &template_id_list));

  for (t = 0; t < template_id_list.num_template_ids; ++t)
  {
    TRY(ppr_get_template_string_by_id(context, gallery, template_id_list.template_ids[t], template_string));
    printf("%04d: %s\n", template_id_list.template_ids[t], template_string);
  }

  ppr_free_gallery(gallery);
  ppr_free_template_id_list(template_id_list);

  ppr_release_context(context);
  ppr_finalize_sdk();
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
