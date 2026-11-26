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
 * TOOL : Trim subject to representative templates
 *
 * Given a gallery file, this program treats the gallery as one subject and
 * writes a new gallery containing only the specified number of representative
 * templates.
 */

#include <stdio.h>
#include <stdlib.h>

#include "pittpatt_ftr_sdk.h"
#include "pittpatt_license.h"
#include "sdk_utils.h"
#include "models_path.h"

/******************************************************************************/

int main(int argc, char *argv[])
{
  ppr_context_type context;
  ppr_gallery_type gallery;
  int num_representative_templates;
  char title[FILENAME_MAX];
  char output_gallery_file_name[FILENAME_MAX];

  if (argc != 3)
  {
    printf("usage: %s <gallery> <num representatives>\n", argv[0]);
    return EXIT_FAILURE;
  }
  num_representative_templates = atoi(argv[2]);

  printf("Initializing...\n");
  
  context = ppr_get_context();
  TRY(ppr_enable_recognition(context));

  TRY(ppr_set_license(context, my_license_id, my_license_key));
  TRY(ppr_set_models_path(context, MODELS_PATH));
  TRY(ppr_set_num_recognition_threads(context, sdk_utils_get_num_cpus()));
  
  TRY(ppr_initialize_context(context));

  printf("Reading gallery %s...\n", argv[1]);  
  TRY(ppr_read_gallery(context, argv[1], &gallery));

  printf("Trimming to representative list of %d templates...\n", num_representative_templates);  
  TRY(ppr_trim_subject_to_representative_templates(context, &gallery, num_representative_templates));

  sdk_utils_get_file_title(argv[1], title);
  sprintf(output_gallery_file_name, "%s_%d_representatives.bin", title, num_representative_templates);
  printf("Writing output gallery %s...\n", output_gallery_file_name);
  TRY(ppr_write_gallery(context, output_gallery_file_name, gallery));
  
  ppr_free_gallery(gallery);
  
  ppr_release_context(context);
  ppr_finalize_sdk();
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
