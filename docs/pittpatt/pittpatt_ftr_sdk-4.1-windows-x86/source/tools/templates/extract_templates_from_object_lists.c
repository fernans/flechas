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
 * TOOL : Extract Templates From Object Lists
 *
 * Given one or more object list files, this program extracts templates from all
 * of the objects for which a template can be generated.
 */

#include <stdio.h>
#include <stdlib.h>

#include "pittpatt_ftr_sdk.h"
#include "pittpatt_license.h"
#include "sdk_utils.h"
#include "models_path.h"

/*******************************************************************************
 * Constants
 ******************************************************************************/

#define TEMPLATE_EXTRACTION_TYPE PPR_EXTRACT_SINGLE

/*******************************************************************************
 * Functions
 ******************************************************************************/

int main(int argc, char *argv[])
{
  int i;
  int i_list;
  ppr_context_type context;
  ppr_object_list_type object_list;
  ppr_raw_image_type raw_image;
  ppr_image_type image;
  ppr_object_suitability_type suitable;
  ppr_template_type curr_template;
  char source_image_name[PPR_MAX_SOURCE_FILE_NAME_SIZE];
  char title[FILENAME_MAX];
  char output_file_name[FILENAME_MAX];
  char str[PPR_MAX_TEMPLATE_STRING_SIZE];

  if (argc < 2)
  {
    printf("usage: %s <object list> [...]\n", argv[0]);
    return EXIT_FAILURE;
  }

  printf("Initializing...\n");
  
  context = ppr_get_context();
  TRY(ppr_enable_recognition(context));

  TRY(ppr_set_license(context, my_license_id, my_license_key));
  TRY(ppr_set_models_path(context, MODELS_PATH));
  TRY(ppr_set_num_recognition_threads(context, sdk_utils_get_num_cpus()));
  TRY(ppr_set_template_extraction_type(context, TEMPLATE_EXTRACTION_TYPE));
  
  TRY(ppr_initialize_context(context));

  for (i_list = 1; i_list < argc; i_list++)
  {
    printf("\n");
    printf("Processing object list[%d] of %d\n", i_list - 1, argc - 1);
    printf("Reading object list %s...\n", argv[i_list]);  
    TRY(ppr_read_object_list(context, argv[i_list], &object_list, source_image_name));

    printf("Reading source image %s...\n", source_image_name);
    TRY_RAW_IMAGE(ppr_raw_image_io_read(source_image_name, &raw_image));
    TRY(ppr_create_image(raw_image, &image));

    sdk_utils_get_file_title(source_image_name, title);
  
    for (i = 0; i < object_list.num_objects; i++)
    {
      printf("Extracting template for object[%d] of %d...\n", i, object_list.num_objects);
    
      TRY(ppr_is_object_suitable_for_recognition(context, object_list.objects[i], &suitable));
      if (suitable)
      {
        TRY(ppr_extract_template_from_object(context, image, object_list.objects[i], &curr_template));
        sprintf(output_file_name, "%s_object_%02d_template.bin", title, i);
        
        TRY(ppr_get_template_string(context, curr_template, str));
        sdk_utils_add_item_to_template_string(str, "#SOURCE", source_image_name);
        TRY(ppr_set_template_string(context, &curr_template, str));
        
        TRY(ppr_write_template(context, output_file_name, curr_template));
        ppr_free_template(curr_template);
      }
      else
      {
        printf("Object not suitable for recognition\n");
      }
    }

    ppr_raw_image_free(raw_image);
    ppr_free_image(image);
    ppr_free_object_list(object_list);
  }

  ppr_release_context(context);
  ppr_finalize_sdk();
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
