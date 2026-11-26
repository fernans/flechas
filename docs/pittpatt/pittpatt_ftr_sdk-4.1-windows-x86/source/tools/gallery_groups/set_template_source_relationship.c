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
 * TOOL : Set Template Source Relationship
 *
 * Given a gallery group file, two gallery IDs, and two template IDs, this
 * program sets the relationship between the templates.
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
  ppr_template_source_type template_source_a;
  ppr_template_source_type template_source_b;
  ppr_context_type context;
  ppr_gallery_group_type gallery_group;
  ppr_template_relationship_type template_relationship;
  
  if (argc != 7)
  {
    fprintf(stderr, "usage: %s <gallery group> <gallery id a> <template id a> <gallery id b> <template id b> <relationship>\n", argv[0]);
    fprintf(stderr, "  where relationship is:\n");
    fprintf(stderr, "    same\n");
    fprintf(stderr, "    diff\n");
    fprintf(stderr, "    unknown\n");
    return EXIT_FAILURE;
  }

  template_source_a.gallery_id  = atoi(argv[2]);
  template_source_a.template_id = atoi(argv[3]);
  template_source_b.gallery_id  = atoi(argv[4]);
  template_source_b.template_id = atoi(argv[5]);

  if (strcmp(argv[6], "same") == 0)
  {
    template_relationship = PPR_RELATIONSHIP_SAME_SUBJECT;
  }
  else if (strcmp(argv[6], "diff") == 0)
  {
    template_relationship = PPR_RELATIONSHIP_DIFFERENT_SUBJECTS;
  }
  else if (strcmp(argv[6], "unknown") == 0)
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

  printf("Reading gallery group %s...\n", argv[1]);
  TRY(ppr_read_gallery_group(context, argv[1], &gallery_group));

  printf("Setting relationship between template %d/%d and template %d/%d...\n",
         template_source_a.gallery_id, template_source_a.template_id,
         template_source_b.gallery_id, template_source_b.template_id);
  
  err = ppr_set_template_source_relationship(context, &gallery_group, template_source_a,
                                             template_source_b, template_relationship);
  if ((err == PPR_NO_GALLERY_WITH_MATCHING_ID) || (err == PPR_NO_TEMPLATE_WITH_MATCHING_ID))
  {
    fprintf(stderr, "Template %d/%d and/or %d/%d not in gallery group %s\n",
            template_source_a.gallery_id, template_source_a.template_id,
            template_source_b.gallery_id, template_source_b.template_id, argv[1]);
    return EXIT_FAILURE;
  }
  sdk_utils_check_ppr_error(err, "ppr_set_template_source_relationship");

  printf("Writing gallery group %s...\n", argv[1]);
  TRY(ppr_write_gallery_group(context, argv[1], gallery_group));
  
  ppr_free_gallery_group(gallery_group);

  ppr_release_context(context);
  ppr_finalize_sdk();
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
