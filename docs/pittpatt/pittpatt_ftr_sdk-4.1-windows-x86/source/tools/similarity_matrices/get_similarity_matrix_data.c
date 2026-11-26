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
 * TOOL : Get Similarity Matrix Data
 *
 * Given a similarity matrix file, prints the values in the matrix.
 */

#include <stdio.h>
#include <stdlib.h>

#include "pittpatt_ftr_sdk.h"
#include "sdk_utils.h"

/******************************************************************************/

int main(int argc, char *argv[])
{
  int i;
  int j;
  ppr_context_type context;
  ppr_similarity_matrix_type matrix;
  ppr_template_id_list_type query_row_template_id_list;
  ppr_template_id_list_type target_col_template_id_list;
  float value;
  
  if (argc != 2)
  {
    fprintf(stderr, "usage: %s <similarity matrix>\n", argv[0]);
    return EXIT_FAILURE;
  }

  context = ppr_get_context();
  TRY(ppr_enable_recognition(context));
  
  printf("Reading similarity_matrix %s...\n", argv[1]);
  TRY(ppr_read_similarity_matrix(context, argv[1], &matrix));

  TRY(ppr_get_query_template_id_list_from_similarity_matrix(context, matrix, &query_row_template_id_list));
  TRY(ppr_get_target_template_id_list_from_similarity_matrix(context, matrix, &target_col_template_id_list));

  for (i = 0; i < query_row_template_id_list.num_template_ids; i++)
  {
    for (j = 0; j < target_col_template_id_list.num_template_ids; j++)
    {
      TRY(ppr_get_similarity_matrix_element(context,
                                            matrix,
                                            query_row_template_id_list.template_ids[i],
                                            target_col_template_id_list.template_ids[j],
                                            &value));
      printf("%10.6f ", value);
    }
    printf("\n");
  }

  ppr_free_template_id_list(query_row_template_id_list);
  ppr_free_template_id_list(target_col_template_id_list);
  ppr_free_similarity_matrix(matrix);

  ppr_release_context(context);
  ppr_finalize_sdk();
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
