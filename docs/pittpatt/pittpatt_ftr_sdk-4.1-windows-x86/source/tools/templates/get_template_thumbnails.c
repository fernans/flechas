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
 * TOOL : Get Template Thumbnails
 *
 * Given one or more template files, this program writes the templates'
 * thumbnails.
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
  int t;
  ppr_context_type context;
  ppr_template_type template;
  ppr_raw_image_type thumbnail;
  char title[FILENAME_MAX];
  char thumbnail_file_name[FILENAME_MAX];
  
  if (argc < 2)
  {
    fprintf(stderr, "usage: %s <template> [...]\n", argv[0]);
    return EXIT_FAILURE;
  }
  
  context = ppr_get_context();
  TRY(ppr_enable_recognition(context));

  for (t = 1; t < argc; t++)
  {
    printf("Reading template %s...\n", argv[t]);  
    TRY(ppr_read_template(context, argv[t], &template));

    printf("Getting thumbnail...\n");
    TRY(ppr_get_template_thumbnail(context, template, &thumbnail));

    sdk_utils_get_file_title(argv[t], title);
    sprintf(thumbnail_file_name, "%s_thumbnail.%s", title, OUTPUT_IMAGE_FORMAT);
    printf("Writing thumbnail %s...\n", thumbnail_file_name);
    TRY_RAW_IMAGE(ppr_raw_image_io_write(thumbnail_file_name, thumbnail));

    ppr_free_template(template);
    ppr_raw_image_free(thumbnail);
  }

  ppr_release_context(context);
  ppr_finalize_sdk();
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
