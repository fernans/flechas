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
 * TOOL : Get Ranked List For Template Source
 *
 * Given a gallery group file, a gallery ID, and a template ID, this
 * program writes ranked-list thumbnails to a directory.
 */

#include <stdio.h>
#include <stdlib.h>

#include "pittpatt_ftr_sdk.h"
#include "pittpatt_raw_image_io.h"
#include "pittpatt_license.h"
#include "sdk_utils.h"
#include "models_path.h"

/*******************************************************************************
 * Constants
 ******************************************************************************/

#define MAX_LIST_LENGTH 20
#define THRESHOLD       0.0f
#define OUTPUT_FORMAT   "jpg"

/*******************************************************************************
 * Functions
 ******************************************************************************/

int main(int argc, char *argv[])
{
  int i;
  ppr_error_type err;
  ppr_template_source_type template_source;
  ppr_context_type context;
  ppr_gallery_group_type gallery_group;
  ppr_template_source_list_type ranked_source_list;
  ppr_score_list_type score_list;
  ppr_gallery_type gallery;
  char title[FILENAME_MAX];
  char directory_name[FILENAME_MAX];
  char file_name[FILENAME_MAX];
  ppr_template_type tmp_template;
  
  if (argc != 4)
  {
    fprintf(stderr, "usage: %s <gallery group> <gallery id> <template id>\n", argv[0]);
    return EXIT_FAILURE;
  }

  template_source.gallery_id  = atoi(argv[2]);
  template_source.template_id = atoi(argv[3]);

  printf("Initializing...\n");
  
  context = ppr_get_context();
  TRY(ppr_enable_recognition(context));

  TRY(ppr_set_license(context, my_license_id, my_license_key));
  TRY(ppr_set_models_path(context, MODELS_PATH));
  TRY(ppr_set_num_recognition_threads(context, sdk_utils_get_num_cpus()));
  
  TRY(ppr_initialize_context(context));

  printf("Reading gallery group %s...\n", argv[1]);
  TRY(ppr_read_gallery_group(context, argv[1], &gallery_group));

  printf("Getting ranked list for template %d/%d...\n", template_source.gallery_id, template_source.template_id);
  
  err = ppr_get_ranked_list_for_template_source(context, gallery_group, template_source, MAX_LIST_LENGTH, THRESHOLD,
                                                &ranked_source_list, &score_list);
  
  if ((err == PPR_NO_GALLERY_WITH_MATCHING_ID) || (err == PPR_NO_TEMPLATE_WITH_MATCHING_ID))
  {
    fprintf(stderr, "Template %d/%d not in gallery group %s\n",
            template_source.gallery_id, template_source.template_id, argv[1]);
    return EXIT_FAILURE;
  }
  sdk_utils_check_ppr_error(err, "ppr_get_ranked_list_for_template_source");

  sdk_utils_get_file_title(argv[1], title);
  sprintf(directory_name, "%s_ranked_list_g%04d_t%04d", title,
          template_source.gallery_id,
          template_source.template_id);
  sdk_utils_make_directory(directory_name);

  for (i = 0; i < ranked_source_list.num_template_sources; ++i)
  {
    TRY(ppr_get_gallery_reference_by_id(context, gallery_group, ranked_source_list.template_sources[i].gallery_id,
                                        &gallery));
    
    TRY(ppr_get_template_by_id(context, gallery, ranked_source_list.template_sources[i].template_id, &tmp_template));
    sprintf(file_name, "%s/r%02d_g%04d_t%04d_score_%.3f.%s", directory_name, i,
            ranked_source_list.template_sources[i].gallery_id,
            ranked_source_list.template_sources[i].template_id, score_list.scores[i], OUTPUT_FORMAT);
    sdk_utils_write_template_thumbnail(file_name, context, tmp_template);
    ppr_free_template(tmp_template);
    
    TRY(ppr_release_gallery_reference(context, gallery_group, &gallery));
  }
  
  ppr_free_template_source_list(ranked_source_list);
  ppr_free_score_list(score_list);
  ppr_free_gallery_group(gallery_group);

  ppr_release_context(context);
  ppr_finalize_sdk();
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
