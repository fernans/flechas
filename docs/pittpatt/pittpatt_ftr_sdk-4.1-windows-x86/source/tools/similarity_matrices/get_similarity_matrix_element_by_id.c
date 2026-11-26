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
 * TOOL : Get Similarity Matrix Element By ID
 *
 * Given a similarity matrix file, prints a single element specified by row and
 * column IDs.
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
  int query_row_id;
  int target_col_id;
  float value;
  
  if (argc != 4)
  {
    fprintf(stderr, "usage: %s <similarity matrix> <query row id> <target col id>\n", argv[0]);
    return EXIT_FAILURE;
  }

  query_row_id = atoi(argv[2]);
  target_col_id = atoi(argv[3]);

  context = ppr_get_context();
  TRY(ppr_enable_recognition(context));
  
  printf("Reading similarity_matrix %s...\n", argv[1]);
  TRY(ppr_read_similarity_matrix(context, argv[1], &matrix));

  printf("Getting element - query row ID = %d, target col ID = %d\n", query_row_id, target_col_id);
  TRY(ppr_get_similarity_matrix_element(context, matrix, query_row_id, target_col_id, &value));
  printf("%f\n", value);

  ppr_free_similarity_matrix(matrix);

  ppr_release_context(context);
  ppr_finalize_sdk();
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
