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
 * TOOL : Get Self-Similarity Matrix
 *
 * Given a gallery file, this program writes the gallery's self-similarity
 * matrix.
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
  ppr_similarity_matrix_type self_similarity_matrix;
  char title[FILENAME_MAX];
  char self_similarity_matrix_file_name[FILENAME_MAX];

  if (argc != 2)
  {
    printf("usage: %s <gallery>\n", argv[0]);
    return EXIT_FAILURE;
  }

  printf("Initializing...\n");
  
  context = ppr_get_context();
  TRY(ppr_enable_recognition(context));

  TRY(ppr_set_license(context, my_license_id, my_license_key));
  TRY(ppr_set_models_path(context, MODELS_PATH));
  TRY(ppr_set_num_recognition_threads(context, sdk_utils_get_num_cpus()));
  
  TRY(ppr_initialize_context(context));

  printf("Reading gallery %s...\n", argv[1]);  
  TRY(ppr_read_gallery(context, argv[1], &gallery));

  printf("Getting self similarity matrix...\n");  
  TRY(ppr_get_self_similarity_matrix(context, gallery, &self_similarity_matrix));

  sdk_utils_get_file_title(argv[1], title);
  sprintf(self_similarity_matrix_file_name, "%s_self_similarity_matrix.bin", title);
  printf("Writing self-similairty matrix %s...\n", self_similarity_matrix_file_name);
  TRY(ppr_write_similarity_matrix(context, self_similarity_matrix_file_name, self_similarity_matrix));
  
  ppr_free_gallery(gallery);
  ppr_free_similarity_matrix(self_similarity_matrix);
  
  ppr_release_context(context);
  ppr_finalize_sdk();
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
