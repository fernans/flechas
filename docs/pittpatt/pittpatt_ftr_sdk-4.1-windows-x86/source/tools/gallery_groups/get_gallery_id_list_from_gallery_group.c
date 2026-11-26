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
 * TOOL : Get Gallery ID List From Gallery Group
 *
 * Given a gallery group file, this prints the IDs of the galleries in the
 * gallery group.
 */

#include <stdio.h>
#include <stdlib.h>

#include "pittpatt_ftr_sdk.h"
#include "sdk_utils.h"

/******************************************************************************/

int main(int argc, char *argv[])
{
  int g;
  ppr_context_type context;
  ppr_gallery_group_type gallery_group;
  ppr_gallery_id_list_type gallery_id_list;
  
  if (argc != 2)
  {
    fprintf(stderr, "usage: %s <gallery group>\n", argv[0]);
    return EXIT_FAILURE;
  }
  
  context = ppr_get_context();
  TRY(ppr_enable_recognition(context));

  printf("Reading gallery group %s...\n", argv[1]);  
  TRY(ppr_read_gallery_group(context, argv[1], &gallery_group));

  printf("Getting gallery id list...\n");  
  TRY(ppr_get_gallery_id_list_from_gallery_group(context, gallery_group, &gallery_id_list));

  printf("Number of Galleries In Gallery Group: %d\n", gallery_id_list.num_gallery_ids);
  printf("IDs:\n");
  for (g = 0; g < gallery_id_list.num_gallery_ids; ++g)
    printf("%d\n", gallery_id_list.gallery_ids[g]);

  ppr_free_gallery_group(gallery_group);
  ppr_free_gallery_id_list(gallery_id_list);

  ppr_release_context(context);
  ppr_finalize_sdk();
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
