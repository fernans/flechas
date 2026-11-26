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
 * TOOL : Get Similarity Matrix Row By Index
 *
 * Given a similarity matrix file, prints an entire row specified by its index.
 */

#include <stdio.h>
#include <stdlib.h>

#include "pittpatt_ftr_sdk.h"
#include "sdk_utils.h"

/******************************************************************************/

int main(int argc, char *argv[])
{
  int i;
  ppr_context_type context;
  ppr_similarity_matrix_type matrix;
  int query_row_index;
  ppr_template_id_list_type query_row_template_id_list;
  int query_row_id;
  ppr_template_id_list_type target_col_template_id_list;
  float value;
  
  if (argc != 3)
  {
    fprintf(stderr, "usage: %s <similarity matrix> <query row index>\n", argv[0]);
    return EXIT_FAILURE;
  }

  query_row_index = atoi(argv[2]);

  context = ppr_get_context();
  TRY(ppr_enable_recognition(context));

  printf("Reading similarity_matrix %s...\n", argv[1]);
  TRY(ppr_read_similarity_matrix(context, argv[1], &matrix));

  printf("Getting row ID for index %d...\n", query_row_index);
  TRY(ppr_get_query_template_id_list_from_similarity_matrix(context, matrix, &query_row_template_id_list));
  if (query_row_index < 0 || query_row_index > query_row_template_id_list.num_template_ids)
  {
    fprintf(stderr, "ERROR: template index %d is out of range\n", query_row_index);
    exit(EXIT_FAILURE);
  }
  query_row_id = query_row_template_id_list.template_ids[query_row_index];
  
  printf("Printing row with ID = %d\n", query_row_id);
  TRY(ppr_get_target_template_id_list_from_similarity_matrix(context, matrix, &target_col_template_id_list));

  for (i = 0; i < target_col_template_id_list.num_template_ids; ++i)
  {
    TRY(ppr_get_similarity_matrix_element(context, matrix, query_row_id, target_col_template_id_list.template_ids[i],
                                          &value));
    printf("%10.6f\n", value);
  }

  ppr_free_template_id_list(query_row_template_id_list);
  ppr_free_template_id_list(target_col_template_id_list);
  ppr_free_similarity_matrix(matrix);

  ppr_release_context(context);
  ppr_finalize_sdk();
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
