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
 * TOOL : Print Object List
 *
 * Given an object list file this program prints its contents in a readable
 * format.
 */

#include <stdio.h>
#include <stdlib.h>

#include "pittpatt_ftr_sdk.h"
#include "sdk_utils.h"

/******************************************************************************/

int main(int argc, char *argv[])
{
  ppr_context_type context;
  ppr_object_list_type object_list;
  char source_image_name[PPR_MAX_SOURCE_FILE_NAME_SIZE];
  
  if (argc != 2)
  {
    fprintf(stderr, "usage: %s <object list>\n", argv[0]);
    return EXIT_FAILURE;
  }
  
  context = ppr_get_context();

  printf("Reading object list %s...\n", argv[1]);  
  TRY(ppr_read_object_list(context, argv[1], &object_list, source_image_name));

  printf("Source Image Name: %s\n", source_image_name);
  sdk_utils_print_object_list(stdout, object_list, 0, 1);

  ppr_free_object_list(object_list);

  ppr_release_context(context);
  ppr_finalize_sdk();
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
