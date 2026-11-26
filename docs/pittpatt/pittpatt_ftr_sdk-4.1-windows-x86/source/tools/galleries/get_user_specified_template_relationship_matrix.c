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
 * TOOL : Get User-Specified Template Relationship Matrix
 *
 * Given a gallery file this program prints a matrix showing all template
 * relationships that were set by the user.
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
  ppr_gallery_type gallery;
  ppr_template_id_list_type template_id_list;
  ppr_template_relationship_type template_relationship;
  
  if (argc != 2)
  {
    fprintf(stderr, "usage: %s <gallery>\n", argv[0]);
    return EXIT_FAILURE;
  }
  
  context = ppr_get_context();
  TRY(ppr_enable_recognition(context));

  printf("Reading gallery %s...\n", argv[1]);  
  TRY(ppr_read_gallery(context, argv[1], &gallery));

  printf("Getting template id list and displaying matrix...\n");
  TRY(ppr_get_template_id_list_from_gallery(context, gallery, &template_id_list));
  
  for (i = 0; i < template_id_list.num_template_ids; i++)
  {
    for (j = 0; j < template_id_list.num_template_ids; j++)
    {
      TRY(ppr_get_user_specified_template_relationship(context,
                                                       gallery,
                                                       template_id_list.template_ids[i],
                                                       template_id_list.template_ids[j],
                                                       &template_relationship));
      switch (template_relationship)
      {
        case PPR_RELATIONSHIP_UNKNOWN:
          printf(". ");
          break;
        case PPR_RELATIONSHIP_SAME_SUBJECT:
          printf("S ");
          break;
        case PPR_RELATIONSHIP_DIFFERENT_SUBJECTS:
          printf("D ");
          break;
        default:
          printf("\n");
          fprintf(stderr, "ERROR: invalid template relationship: %d\n", template_relationship);
          exit(EXIT_FAILURE);
      }
    }
    printf("\n");
  }

  ppr_free_gallery(gallery);
  ppr_free_template_id_list(template_id_list);

  ppr_release_context(context);
  ppr_finalize_sdk();
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
