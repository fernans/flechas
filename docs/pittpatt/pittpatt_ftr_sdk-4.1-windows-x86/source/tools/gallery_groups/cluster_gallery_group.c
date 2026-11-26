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
 * TOOL : Cluster Gallery Group
 *
 * Given a gallery group file, this program performs multi-gallery clustering
 * to produce an inter-gallery subject list.
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

#define NUM_REPRESENTATIVE_TEMPLATES 2
#define STRAGGLER_CUTOFF             0

/*******************************************************************************
 * Local Function Prototypes
 ******************************************************************************/

static void write_inter_gallery_subject_list_thumbnails(
    const char *directory,
    ppr_context_type context,
    ppr_gallery_group_type gallery_group,
    ppr_inter_gallery_subject_list_type inter_gallery_subject_list);

static void write_template_source_list_thumbnails(
    const char *directory,
    ppr_context_type context,
    ppr_gallery_group_type gallery_group,
    ppr_template_source_list_type template_source_list);

/*******************************************************************************
 * Functions
 ******************************************************************************/

int main(int argc, char *argv[])
{
  ppr_context_type context;
  ppr_gallery_group_type gallery_group;
  ppr_inter_gallery_subject_list_type inter_gallery_subject_list;
  int aggressiveness;
  char scratch_directory[FILENAME_MAX];
  
  if (argc < 4 || argc > 5)
  {
    fprintf(stderr, "usage: %s <gallery group> <aggressiveness> <output directory>\n", argv[0]);
    return EXIT_FAILURE;
  }

  aggressiveness  = atoi(argv[2]);

  sdk_utils_make_directory(argv[3]);
  sprintf(scratch_directory, "%s/scratch_directory", argv[3]);
  sdk_utils_make_directory(scratch_directory);

  printf("Initializing...\n");
  
  context = ppr_get_context();
  TRY(ppr_enable_recognition(context));

  TRY(ppr_set_license(context, my_license_id, my_license_key));
  TRY(ppr_set_models_path(context, MODELS_PATH));
  TRY(ppr_set_num_recognition_threads(context, sdk_utils_get_num_cpus()));
  
  TRY(ppr_initialize_context(context));

  printf("Reading gallery group %s...\n", argv[1]);
  TRY(ppr_read_gallery_group(context, argv[1], &gallery_group));

  TRY(ppr_set_num_representative_templates_per_subject(context, &gallery_group, NUM_REPRESENTATIVE_TEMPLATES));
  TRY(ppr_set_straggler_cutoff(context, &gallery_group, STRAGGLER_CUTOFF));
  
  printf("Clustering gallery group...\n");
  TRY(ppr_cluster_gallery_group(context, gallery_group, aggressiveness, &inter_gallery_subject_list));

  printf("Writing inter-gallery subject list thumbnails...\n");
  write_inter_gallery_subject_list_thumbnails(argv[3], context, gallery_group, inter_gallery_subject_list);

  ppr_free_gallery_group(gallery_group);
  ppr_free_inter_gallery_subject_list(inter_gallery_subject_list);

  ppr_release_context(context);
  ppr_finalize_sdk();
  
  return EXIT_SUCCESS;
}

/*****************************************************************************/

static void write_inter_gallery_subject_list_thumbnails(
    const char *directory,
    ppr_context_type context,
    ppr_gallery_group_type gallery_group,
    ppr_inter_gallery_subject_list_type inter_gallery_subject_list)
{
  int i;
  char directory_name[FILENAME_MAX];

  for (i = 0; i < inter_gallery_subject_list.num_subjects; ++i)
  {
    printf(" - Writing subject %04d\n", i);
    sprintf(directory_name, "%s/subject_%04d", directory, i);
    write_template_source_list_thumbnails(directory_name, context, gallery_group,
                                          inter_gallery_subject_list.subjects[i]);
  }
}

/*****************************************************************************/

static void write_template_source_list_thumbnails(
    const char *directory,
    ppr_context_type context,
    ppr_gallery_group_type gallery_group,
    ppr_template_source_list_type template_source_list)
{
  int i;  
  int gallery_id;
  int template_id;
  ppr_gallery_type gallery;
  ppr_template_type tmp_template;
  char thumbnail_filename[FILENAME_MAX];
  
  sdk_utils_make_directory(directory);

  for (i = 0; i < template_source_list.num_template_sources; ++i)
  {
    gallery_id = template_source_list.template_sources[i].gallery_id;
    template_id = template_source_list.template_sources[i].template_id;
    
    TRY(ppr_get_gallery_reference_by_id(context, gallery_group, gallery_id, &gallery));
    
    TRY(ppr_get_template_by_id(context, gallery, template_id, &tmp_template));
    sprintf(thumbnail_filename, "%s/thumbnail_%04d_g_%04d_t_%04d.jpg", directory, i, gallery_id, template_id);
    sdk_utils_write_template_thumbnail(thumbnail_filename, context, tmp_template);
    ppr_free_template(tmp_template);

    TRY(ppr_release_gallery_reference(context, gallery_group, &gallery));
  }
}

/*****************************************************************************/
