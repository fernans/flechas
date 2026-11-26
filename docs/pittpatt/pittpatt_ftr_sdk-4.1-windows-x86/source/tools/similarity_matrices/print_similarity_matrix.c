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
 * TOOL : Print Similarity Matrix
 *
 * Given a similarity matrix file, prints the matrix and its corresponding
 * data in a readable format.
 */

#include <stdio.h>
#include <stdlib.h>

#include "pittpatt_ftr_sdk.h"
#include "sdk_utils.h"

/******************************************************************************/

int main(int argc, char *argv[])
{
  ppr_context_type context;
  ppr_similarity_matrix_type matrix;
  
  if (argc != 2)
  {
    fprintf(stderr, "usage: %s <similarity matrix>\n", argv[0]);
    return EXIT_FAILURE;
  }

  context = ppr_get_context();
  TRY(ppr_enable_recognition(context));
  
  printf("Reading similarity_matrix %s...\n", argv[1]);
  TRY(ppr_read_similarity_matrix(context, argv[1], &matrix));

  sdk_utils_print_similarity_matrix(stdout, context, matrix, 1);

  ppr_free_similarity_matrix(matrix);

  ppr_release_context(context);
  ppr_finalize_sdk();
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
