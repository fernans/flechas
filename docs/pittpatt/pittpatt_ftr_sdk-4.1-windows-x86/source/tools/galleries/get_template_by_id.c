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
 * TOOL : Get Template By ID
 *
 * Given a gallery file and a template ID, this program writes a template file.
 */

#include <stdio.h>
#include <stdlib.h>

#include "pittpatt_ftr_sdk.h"
#include "sdk_utils.h"

/******************************************************************************/

int main(int argc, char *argv[])
{
  ppr_error_type err;
  int template_id;
  ppr_context_type context;
  ppr_gallery_type gallery;
  ppr_template_type template;
  char title[FILENAME_MAX];
  char template_file_name[FILENAME_MAX];
  
  if (argc != 3)
  {
    fprintf(stderr, "usage: %s <gallery> <template id>\n", argv[0]);
    return EXIT_FAILURE;
  }

  template_id = atoi(argv[2]);
  
  context = ppr_get_context();
  TRY(ppr_enable_recognition(context));

  printf("Reading gallery %s...\n", argv[1]);
  TRY(ppr_read_gallery(context, argv[1], &gallery));

  printf("Getting template with ID = %d...\n", template_id);
  
  err = ppr_get_template_by_id(context, gallery, template_id, &template);
  if (err == PPR_NO_TEMPLATE_WITH_MATCHING_ID)
  {
    fprintf(stderr, "Gallery %s has no template with the ID %d\n", argv[1], template_id);
    return EXIT_FAILURE;
  }
  sdk_utils_check_ppr_error(err, "ppr_get_template_by_id");

  sdk_utils_get_file_title(argv[1], title);
  sprintf(template_file_name, "%s_template_%04d.bin", title, template_id);
  printf("Writing template %s...\n", template_file_name);
  
  TRY(ppr_write_template(context, template_file_name, template));

  ppr_free_gallery(gallery);
  ppr_free_template(template);

  ppr_release_context(context);
  ppr_finalize_sdk();
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
