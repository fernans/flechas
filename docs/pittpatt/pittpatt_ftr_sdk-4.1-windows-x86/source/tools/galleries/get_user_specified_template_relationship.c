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
 * TOOL : Get User-Specified Template Relationship
 *
 * Given a gallery file and two template IDs, this program prints the
 * relationship between the templates that was set by the user.
 */

#include <stdio.h>
#include <stdlib.h>

#include "pittpatt_ftr_sdk.h"
#include "sdk_utils.h"

/******************************************************************************/

int main(int argc, char *argv[])
{
  ppr_error_type err;
  int template_id_a;
  int template_id_b;
  ppr_context_type context;
  ppr_gallery_type gallery;
  ppr_template_relationship_type template_relationship;
  
  if (argc != 4)
  {
    fprintf(stderr, "usage: %s <gallery> <template id a> <template id b>\n", argv[0]);
    return EXIT_FAILURE;
  }

  template_id_a = atoi(argv[2]);
  template_id_b = atoi(argv[3]);
  
  context = ppr_get_context();
  TRY(ppr_enable_recognition(context));

  printf("Reading gallery %s...\n", argv[1]);
  TRY(ppr_read_gallery(context, argv[1], &gallery));

  printf("Getting relationship between template ID = %d and template ID = %d...\n", template_id_a, template_id_b);
  
  err = ppr_get_user_specified_template_relationship(context, gallery, template_id_a, template_id_b,
                                                     &template_relationship);
  if (err == PPR_NO_TEMPLATE_WITH_MATCHING_ID)
  {
    fprintf(stderr, "Template ID %d and/or %d not in gallery %s\n", template_id_a, template_id_b, argv[1]);
    return EXIT_FAILURE;
  }
  sdk_utils_check_ppr_error(err, "ppr_get_user_specified_template_relationship");

  switch (template_relationship)
  {
    case PPR_RELATIONSHIP_SAME_SUBJECT:
      printf("SAME SUBJECT\n");
      break;
    case PPR_RELATIONSHIP_DIFFERENT_SUBJECTS:
      printf("DIFFERENT SUBJECTS\n");
      break;
    case PPR_RELATIONSHIP_UNKNOWN:
      printf("UNKNOWN (or never set)\n");
      break;
    default:
      fprintf(stderr, "ILLEGAL RELATIONSHIP: %d\n", (int)template_relationship);
      return EXIT_FAILURE;
  }

  ppr_free_gallery(gallery);

  ppr_release_context(context);
  ppr_finalize_sdk();
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
