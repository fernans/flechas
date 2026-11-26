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
 * TOOL : Get User-Specified Template Source Relationship
 *
 * Given a gallery group file, two gallery IDs, and two template IDs, this
 * program prints the relationship between the templates that was set by the
 * user.
 */

#include <stdio.h>
#include <stdlib.h>

#include "pittpatt_ftr_sdk.h"
#include "sdk_utils.h"

/******************************************************************************/

int main(int argc, char *argv[])
{
  ppr_error_type err;
  ppr_template_source_type template_source_a;
  ppr_template_source_type template_source_b;
  ppr_context_type context;
  ppr_gallery_group_type gallery_group;
  ppr_template_relationship_type template_relationship;
  
  if (argc != 6)
  {
    fprintf(stderr, "usage: %s <gallery group> <gallery id a> <template id a> <gallery id b> <template id b>\n", argv[0]);
    return EXIT_FAILURE;
  }

  template_source_a.gallery_id  = atoi(argv[2]);
  template_source_a.template_id = atoi(argv[3]);
  template_source_b.gallery_id  = atoi(argv[4]);
  template_source_b.template_id = atoi(argv[5]);
  
  context = ppr_get_context();
  TRY(ppr_enable_recognition(context));

  printf("Reading gallery group %s...\n", argv[1]);
  TRY(ppr_read_gallery_group(context, argv[1], &gallery_group));

  printf("Getting relationship between template %d/%d and template %d/%d...\n",
         template_source_a.gallery_id, template_source_a.template_id,
         template_source_b.gallery_id, template_source_b.template_id);
  
  err = ppr_get_user_specified_template_source_relationship(context, gallery_group, template_source_a,
                                                            template_source_b, &template_relationship);
  if ((err == PPR_NO_GALLERY_WITH_MATCHING_ID) || (err == PPR_NO_TEMPLATE_WITH_MATCHING_ID))
  {
    fprintf(stderr, "Template %d/%d and/or %d/%d not in gallery group %s\n",
            template_source_a.gallery_id, template_source_a.template_id,
            template_source_b.gallery_id, template_source_b.template_id, argv[1]);
    return EXIT_FAILURE;
  }
  sdk_utils_check_ppr_error(err, "ppr_get_user_specified_template_source_relationship");

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

  ppr_free_gallery_group(gallery_group);

  ppr_release_context(context);
  ppr_finalize_sdk();
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
