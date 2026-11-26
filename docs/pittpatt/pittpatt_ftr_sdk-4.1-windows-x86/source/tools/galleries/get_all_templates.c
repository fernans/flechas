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
 * TOOL : Get All Templates
 *
 * Given a gallery file, this program writes a template file for every template
 * in the gallery.
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
  ppr_template_type template;
  char title[FILENAME_MAX];
  char template_file_name[FILENAME_MAX];
  
  if (argc != 2)
  {
    printf("usage: %s <gallery>\n", argv[0]);
    return EXIT_FAILURE;
  }

  context = ppr_get_context();
  TRY(ppr_enable_recognition(context));

  printf("Reading gallery %s...\n", argv[1]);
  TRY(ppr_read_gallery(context, argv[1], &gallery));

  printf("Getting template id list...\n");  
  TRY(ppr_get_template_id_list_from_gallery(context, gallery, &template_id_list));

  sdk_utils_get_file_title(argv[1], title);
  
  for (t = 0; t < template_id_list.num_template_ids; ++t)
  {
    printf("Processing template[%d] of %d\n", t, template_id_list.num_template_ids);
    
    printf("Getting template with ID = %d...\n", template_id_list.template_ids[t]);  
    TRY(ppr_get_template_by_id(context, gallery, template_id_list.template_ids[t], &template));

    sprintf(template_file_name, "%s_template_%04d.bin", title, template_id_list.template_ids[t]);
    
    printf("Writing template %s...\n", template_file_name);  
    TRY(ppr_write_template(context, template_file_name, template));

    ppr_free_template(template);
  }

  ppr_free_gallery(gallery);
  ppr_free_template_id_list(template_id_list);
  
  ppr_release_context(context);
  ppr_finalize_sdk();
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
