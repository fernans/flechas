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
 * TOOL : Compare Galleries
 *
 * Given two gallery files, this program compares them and writes a similarity
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
  ppr_gallery_type gallery_a;
  ppr_gallery_type gallery_b;
  ppr_similarity_matrix_type similarity_matrix;
  char gallery_a_title[FILENAME_MAX];
  char gallery_b_title[FILENAME_MAX];
  char similarity_matrix_file_name[FILENAME_MAX];

  if (argc != 3)
  {
    printf("usage: %s <query gallery> <target gallery>\n", argv[0]);
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
  TRY(ppr_read_gallery(context, argv[1], &gallery_a));

  printf("Reading gallery %s...\n", argv[2]);
  TRY(ppr_read_gallery(context, argv[2], &gallery_b));

  printf("Comparing galleries...\n");
  
  TRY(ppr_compare_galleries(context, gallery_a, gallery_b, &similarity_matrix));

  sdk_utils_get_file_title(argv[1], gallery_a_title);
  sdk_utils_get_file_title(argv[2], gallery_b_title);
  sprintf(similarity_matrix_file_name, "%s_to_%s_similarity_matrix.bin", gallery_a_title, gallery_b_title);
  TRY(ppr_write_similarity_matrix(context, similarity_matrix_file_name, similarity_matrix));

  ppr_free_gallery(gallery_a);
  ppr_free_gallery(gallery_b);
  ppr_free_similarity_matrix(similarity_matrix);
  
  ppr_release_context(context);
  ppr_finalize_sdk();
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
