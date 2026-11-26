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
 * TOOL : Compare Template To Gallery
 *
 * Given a template file and a gallery file, this program compares them and 
 * prints the scores to the screen.
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
  ppr_template_type template;
  ppr_gallery_type gallery;
  ppr_score_list_type score_list;
  int i;

  if (argc != 3)
  {
    printf("usage: %s <query template> <target gallery>\n", argv[0]);
    return EXIT_FAILURE;
  }

  printf("Initializing...\n");
  
  context = ppr_get_context();
  TRY(ppr_enable_recognition(context));

  TRY(ppr_set_license(context, my_license_id, my_license_key));
  TRY(ppr_set_models_path(context, MODELS_PATH));
  TRY(ppr_set_num_recognition_threads(context, sdk_utils_get_num_cpus()));
  
  TRY(ppr_initialize_context(context));

  printf("Reading template %s...\n", argv[1]);  
  TRY(ppr_read_template(context, argv[1], &template));

  printf("Reading gallery %s...\n", argv[2]);
  TRY(ppr_read_gallery(context, argv[2], &gallery));

  printf("Comparing template to gallery...\n");
  
  TRY(ppr_compare_template_to_gallery(context, template, gallery, &score_list));

  printf("scores: ");
  for (i = 0; i < score_list.num_scores; i++)
    printf("%.3f  ", score_list.scores[i]);
  printf("\n");

  ppr_free_template(template);
  ppr_free_gallery(gallery);
  ppr_free_score_list(score_list);
  
  ppr_release_context(context);
  ppr_finalize_sdk();
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
