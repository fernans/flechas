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
 * TOOL : Set Template Relationship
 *
 * Given a gallery file and two template IDs, this program sets the
 * relationship between the templates.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  
  if (argc != 5)
  {
    fprintf(stderr, "usage: %s <gallery> <template id a> <template id b> <relationship>\n", argv[0]);
    fprintf(stderr, "  where relationship is:\n");
    fprintf(stderr, "    same\n");
    fprintf(stderr, "    diff\n");
    fprintf(stderr, "    unknown\n");

    return EXIT_FAILURE;
  }

  template_id_a = atoi(argv[2]);
  template_id_b = atoi(argv[3]);

  if (strcmp(argv[4], "same") == 0)
  {
    template_relationship = PPR_RELATIONSHIP_SAME_SUBJECT;
  }
  else if (strcmp(argv[4], "diff") == 0)
  {
    template_relationship = PPR_RELATIONSHIP_DIFFERENT_SUBJECTS;
  }
  else if (strcmp(argv[4], "unknown") == 0)
  {
    template_relationship = PPR_RELATIONSHIP_UNKNOWN;
  }
  else
  {
    fprintf(stderr, "Invalid relationship: %s\n", argv[4]);
    fprintf(stderr, "  Valid options are:\n");
    fprintf(stderr, "    same\n");
    fprintf(stderr, "    diff\n");
    fprintf(stderr, "    unknown\n");
    
    return EXIT_FAILURE;
  }
  
  context = ppr_get_context();
  TRY(ppr_enable_recognition(context));

  printf("Reading gallery %s...\n", argv[1]);
  TRY(ppr_read_gallery(context, argv[1], &gallery));

  printf("Setting relationship between template ID = %d and template ID = %d...\n", template_id_a, template_id_b);
  
  err = ppr_set_template_relationship(context, &gallery, template_id_a, template_id_b, template_relationship);
  if (err == PPR_NO_TEMPLATE_WITH_MATCHING_ID)
  {
    fprintf(stderr, "Template ID %d and/or %d not in gallery %s\n", template_id_a, template_id_b, argv[1]);
    return EXIT_FAILURE;
  }
  sdk_utils_check_ppr_error(err, "ppr_set_template_relationship");

  printf("Writing gallery %s...\n", argv[1]);
  TRY(ppr_write_gallery(context, argv[1], gallery));

  ppr_free_gallery(gallery);

  ppr_release_context(context);
  ppr_finalize_sdk();
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
