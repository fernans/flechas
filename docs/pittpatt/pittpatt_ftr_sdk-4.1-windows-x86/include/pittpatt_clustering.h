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


#ifndef __PITTPATT_CLUSTERING_H
#define __PITTPATT_CLUSTERING_H

#include "pittpatt_detection.h"
#include "pittpatt_recognition.h"
#include "pittpatt_errors.h"

/* If C++ then we need to extern "C". Compiler defines __cplusplus */
#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Constants
 *****************************************************************************/

#define PPR_MAX_CLUSTERING_AGGRESSIVENESS 10
  
/*******************************************************************************
 * Basic Types
 ******************************************************************************/

typedef struct ppr_gallery_group_struct* ppr_gallery_group_type;

/*******************************************************************************
 * Structure Types
 ******************************************************************************/

typedef struct
{
  size_t num_bytes;
  void *data;
} ppr_flat_gallery_group_type;

typedef struct
{
  int num_gallery_ids;
  int *gallery_ids;
  int capacity;
} ppr_gallery_id_list_type;

typedef struct
{
  int gallery_id;
  int template_id;
} ppr_template_source_type;

typedef struct
{
  int num_template_sources;
  ppr_template_source_type *template_sources;
  int capacity;
} ppr_template_source_list_type;

typedef struct
{
  int num_subjects;
  ppr_template_source_list_type *subjects;
  int capacity;
} ppr_inter_gallery_subject_list_type;

/*******************************************************************************
 * Gallery Group Building and Editing Functions
 ******************************************************************************/

ppr_error_type ppr_create_gallery_group (ppr_context_type context,
                                         ppr_gallery_group_type *gallery_group);

ppr_error_type ppr_create_disk_gallery_group (ppr_context_type context,
                                              ppr_gallery_group_type *gallery_group,
                                              const char *scratch_directory);

ppr_error_type ppr_move_gallery_to_gallery_group (ppr_context_type context,
                                                  ppr_gallery_group_type *gallery_group,
                                                  ppr_gallery_type *gallery,
                                                  int *gallery_id);

ppr_error_type ppr_remove_gallery_from_gallery_group (ppr_context_type context,
                                                      ppr_gallery_group_type *gallery_group,
                                                      int gallery_id);

ppr_error_type ppr_set_template_source_relationship (ppr_context_type context,
                                                     ppr_gallery_group_type *gallery_group,
                                                     ppr_template_source_type template_source_a,
                                                     ppr_template_source_type template_source_b,
                                                     ppr_template_relationship_type relationship);

ppr_error_type ppr_set_gallery_string_by_id (ppr_context_type context,
                                             ppr_gallery_group_type *gallery_group,
                                             int gallery_id,
                                             const char *string);

/*******************************************************************************
 * Configure Functions
 ******************************************************************************/

ppr_error_type ppr_set_num_representative_templates_per_subject (ppr_context_type context,
                                                                 ppr_gallery_group_type *gallery_group,
                                                                 int num_representative_templates);

ppr_error_type ppr_set_straggler_cutoff (ppr_context_type context,
                                         ppr_gallery_group_type *gallery_group,
                                         int straggler_cutoff);
  
/*******************************************************************************
 * Query Functions
 ******************************************************************************/

ppr_error_type ppr_get_num_representative_templates (ppr_context_type context,
                                                     ppr_gallery_group_type gallery_group,
                                                     int *num_representative_templates);
  
ppr_error_type ppr_get_straggler_cutoff (ppr_context_type context,
                                         ppr_gallery_group_type gallery_group,
                                         int *straggler_cutoff);

ppr_error_type ppr_get_gallery_id (ppr_context_type context,
                                   ppr_gallery_type gallery,
                                   int *gallery_id);

ppr_error_type ppr_get_num_galleries_in_gallery_group (ppr_context_type context,
                                                       ppr_gallery_group_type gallery_group,
                                                       int *num_galleries);

ppr_error_type ppr_get_gallery_id_list_from_gallery_group (ppr_context_type context,
                                                           ppr_gallery_group_type gallery_group,
                                                           ppr_gallery_id_list_type *gallery_id_list);

ppr_error_type ppr_get_gallery_reference_by_id (ppr_context_type context,
                                                ppr_gallery_group_type gallery_group,
                                                int gallery_id,
                                                ppr_gallery_type *gallery);

ppr_error_type ppr_release_gallery_reference (ppr_context_type context,
                                              ppr_gallery_group_type gallery_group,
                                              ppr_gallery_type *gallery);

ppr_error_type ppr_get_gallery_string_length_by_id (ppr_context_type context,
                                                    ppr_gallery_group_type gallery_group,
                                                    int gallery_id,
                                                    size_t *string_length);

ppr_error_type ppr_get_gallery_string_by_id (ppr_context_type context,
                                             ppr_gallery_group_type gallery_group,
                                             int gallery_id,
                                             char *string);

ppr_error_type ppr_get_user_specified_template_source_relationship (ppr_context_type context,
                                                                    ppr_gallery_group_type gallery_group,
                                                                    ppr_template_source_type template_source_a,
                                                                    ppr_template_source_type template_source_b,
                                                                    ppr_template_relationship_type *relationship);

/*******************************************************************************
 * Clustering and Comparison Functions
 ******************************************************************************/

ppr_error_type ppr_cluster_gallery (ppr_context_type context,
                                    ppr_gallery_type gallery,
                                    int clustering_aggressiveness,
                                    ppr_subject_list_type *subject_list);

ppr_error_type ppr_cluster_gallery_group (ppr_context_type context,
                                          ppr_gallery_group_type gallery_group,
                                          int clustering_aggressiveness,
                                          ppr_inter_gallery_subject_list_type *inter_gallery_subject_list);

ppr_error_type ppr_get_ranked_list_for_template_source (ppr_context_type context,
                                                        ppr_gallery_group_type gallery_group,
                                                        ppr_template_source_type query_template_source,
                                                        int max_list_length,
                                                        float threshold,
                                                        ppr_template_source_list_type *ranked_source_list,
                                                        ppr_score_list_type *scores);

/*******************************************************************************
 * File I/O Functions
 ******************************************************************************/

ppr_error_type ppr_write_gallery_group (ppr_context_type context,
                                        const char *file_name,
                                        ppr_gallery_group_type gallery_group);

ppr_error_type ppr_read_gallery_group (ppr_context_type context,
                                       const char *file_name,
                                       ppr_gallery_group_type *gallery_group);

ppr_error_type ppr_read_as_disk_gallery_group (ppr_context_type context,
                                               const char *file_name,
                                               ppr_gallery_group_type *gallery_group,
                                               const char *scratch_directory);

/*******************************************************************************
 * Flatten / Unflatten Functions
 ******************************************************************************/

ppr_error_type ppr_flatten_gallery_group (ppr_context_type context,
                                          ppr_gallery_group_type gallery_group,
                                          ppr_flat_gallery_group_type *flat_gallery_group);

ppr_error_type ppr_unflatten_gallery_group (ppr_context_type context,
                                            ppr_flat_gallery_group_type flat_gallery_group,
                                            ppr_gallery_group_type *gallery_group);

ppr_error_type ppr_unflatten_as_disk_gallery_group (ppr_context_type context,
                                                    ppr_flat_gallery_group_type flat_gallery_group,
                                                    ppr_gallery_group_type *gallery_group,
                                                    const char *scratch_directory);

/*******************************************************************************
 * Free Functions
 ******************************************************************************/

void ppr_free_gallery_group (ppr_gallery_group_type gallery_group);

void ppr_free_flat_gallery_group (ppr_flat_gallery_group_type flat_gallery_group);

void ppr_free_gallery_id_list (ppr_gallery_id_list_type gallery_id_list);
  
void ppr_free_template_source_list (ppr_template_source_list_type template_source_list);

void ppr_free_inter_gallery_subject_list (ppr_inter_gallery_subject_list_type inter_gallery_subject_list);

/******************************************************************************/
  
#ifdef __cplusplus
}
#endif
  
#endif /* __PITTPATT_CLUSTERING_H */
