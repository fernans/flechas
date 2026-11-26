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
 * TOOL : Crop Objects From Lists
 *
 * Given one or more object list files this program crops the objects out of
 * their images and writes images to disk.
 */

#include <stdio.h>
#include <stdlib.h>

#include "pittpatt_ftr_sdk.h"
#include "pittpatt_raw_image_io.h"
#include "sdk_utils.h"

/*******************************************************************************
 * Constants
 ******************************************************************************/

#define OUTPUT_IMAGE_FORMAT "jpg"

/*******************************************************************************
 * Functions
 ******************************************************************************/

int main(int argc, char *argv[])
{
  int i_list;
  int i;
  ppr_context_type context;
  ppr_object_list_type object_list;
  char source_image_name[PPR_MAX_SOURCE_FILE_NAME_SIZE];
  char title[FILENAME_MAX];
  char output_image_name[FILENAME_MAX];
  ppr_raw_image_type raw_image;
  ppr_image_type image;
  ppr_raw_image_type cropped_image;
  
  if (argc < 2)
  {
    fprintf(stderr, "usage: %s <object list> [...]\n", argv[0]);
    return EXIT_FAILURE;
  }
  
  context = ppr_get_context();

  for (i_list = 1; i_list < argc; i_list++)
  {
    printf("Reading object list %s...\n", argv[i_list]);  
    TRY(ppr_read_object_list(context, argv[i_list], &object_list, source_image_name));

    printf("Reading source image %s...\n", source_image_name);
    TRY_RAW_IMAGE(ppr_raw_image_io_read(source_image_name, &raw_image));
    TRY(ppr_create_image(raw_image, &image));

    sdk_utils_get_file_title(source_image_name, title);
  
    for (i = 0; i < object_list.num_objects; ++i)
    {
      printf("Cropping object[%d] of %d...\n", i, object_list.num_objects);
      TRY(ppr_crop_object_from_image(context, image, object_list.objects[i], &cropped_image));
      
      sprintf(output_image_name, "%s_object_%02d.%s", title, i, OUTPUT_IMAGE_FORMAT);
      printf("Writing object image %s...\n", output_image_name);
      TRY_RAW_IMAGE(ppr_raw_image_io_write(output_image_name, cropped_image));

      ppr_raw_image_free(cropped_image);
    }

    ppr_free_image(image);
    ppr_raw_image_free(raw_image);
    ppr_free_object_list(object_list);
  }

  ppr_release_context(context);
  ppr_finalize_sdk();
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
