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
 * TOOL : Overlay Object Lists
 *
 * Given one or more object list files this program overlays polygons onto the
 * source images.
 */

#include <stdio.h>
#include <stdlib.h>

#include "pittpatt_ftr_sdk.h"
#include "pittpatt_raw_image_io.h"
#include "sdk_utils.h"

/*******************************************************************************
 * Constants
 ******************************************************************************/

#define OUTPUT_IMAGE_FORMAT        "jpg"
#define COLORS_BASED_ON_CONFIDENCE 1

/*******************************************************************************
 * Functions
 ******************************************************************************/

int main(int argc, char *argv[])
{
  int i_list;
  ppr_context_type context;
  ppr_object_list_type object_list;
  ppr_raw_image_type raw_image;
  char source_image_name[PPR_MAX_SOURCE_FILE_NAME_SIZE];
  char title[FILENAME_MAX];
  char out_file_name[FILENAME_MAX];
  
  if (argc < 2)
  {
    fprintf(stderr, "usage: %s <object list> [...]\n", argv[0]);
    return EXIT_FAILURE;
  }
  
  context = ppr_get_context();

  for (i_list = 1; i_list < argc; i_list++)
  {
    printf("\n");
    printf("Processing object list[%d] of %d\n", i_list - 1, argc - 1);
    
    printf("Reading object list %s...\n", argv[i_list]);  
    TRY(ppr_read_object_list(context, argv[i_list], &object_list, source_image_name));

    printf("Reading source image %s...\n", source_image_name);
    TRY_RAW_IMAGE(ppr_raw_image_io_read(source_image_name, &raw_image));

    printf("Overlaying objects...\n");
    sdk_utils_overlay_object_list(&raw_image, object_list, COLORS_BASED_ON_CONFIDENCE);
  
    sdk_utils_get_file_title(source_image_name, title);
    sprintf(out_file_name, "%s_object_list_overlay.%s", title, OUTPUT_IMAGE_FORMAT);
    printf("Writing output image %s...\n", out_file_name);
    TRY_RAW_IMAGE(ppr_raw_image_io_write(out_file_name, raw_image));

    ppr_raw_image_free(raw_image);
    ppr_free_object_list(object_list);
  }

  ppr_release_context(context);
  ppr_finalize_sdk();
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
