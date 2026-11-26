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


#ifndef __PITTPATT_RECOGNITION_H
#define __PITTPATT_RECOGNITION_H

#include <stddef.h>

#include "pittpatt_detection.h"
#include "pittpatt_raw_image.h"
#include "pittpatt_errors.h"

/* If C++ then we need to extern "C". Compiler defines __cplusplus */
#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Constants
 *****************************************************************************/

#define PPR_MAX_TEMPLATE_STRING_SIZE 4096  /* Including null terminator */
#define PPR_MAX_GALLERY_STRING_SIZE  4096  /* Including null terminator */

/*****************************************************************************
 * Basic Types
 *****************************************************************************/

typedef struct ppr_template_struct* ppr_template_type;
typedef struct ppr_gallery_struct* ppr_gallery_type;
typedef struct ppr_similarity_matrix_struct* ppr_similarity_matrix_type;

/*****************************************************************************
 * Enumerated Types
 *****************************************************************************/

typedef enum
{
  PPR_EXTRACT_SINGLE,
  PPR_EXTRACT_DOUBLE
} ppr_template_extraction_type;

typedef enum
{
  PPR_RELATIONSHIP_UNKNOWN,
  PPR_RELATIONSHIP_SAME_SUBJECT,
  PPR_RELATIONSHIP_DIFFERENT_SUBJECTS
} ppr_template_relationship_type;

typedef enum
{
  PPR_OBJECT_NOT_SUITABLE_FOR_RECOGNITION,
  PPR_OBJECT_SUITABLE_FOR_RECOGNITION
} ppr_object_suitability_type;

/*****************************************************************************
 * Structure Types
 *****************************************************************************/

typedef struct
{
  size_t num_bytes;
  void *data;
} ppr_flat_template_type;

typedef struct
{
  size_t num_bytes;
  void *data;
} ppr_flat_gallery_type;

typedef struct
{
  size_t num_bytes;
  void *data;
} ppr_flat_similarity_matrix_type;

typedef struct
{
  int num_template_ids;
  int *template_ids;
  int capacity;
} ppr_template_id_list_type;

typedef struct
{
  int num_indices;
  int *indices;
  int capacity;
} ppr_index_list_type;

typedef struct
{
  int num_scores;
  float *scores;
  int capacity;
} ppr_score_list_type;

typedef struct
{
  int num_subjects;
  ppr_template_id_list_type *subjects;
  int capacity;
} ppr_subject_list_type;

/*****************************************************************************
 * Initialize and Configure Functions
 *****************************************************************************/

ppr_error_type ppr_enable_recognition (ppr_context_type context);

ppr_error_type ppr_set_num_recognition_threads (ppr_context_type context,
                                                int num_threads);

ppr_error_type ppr_set_template_extraction_type (ppr_context_type context,
                                                 ppr_template_extraction_type extraction_type);

ppr_error_type ppr_disable_template_thumbnails (ppr_context_type context);

/*****************************************************************************
 * Context Query Functions
 *****************************************************************************/

ppr_error_type ppr_is_recognition_enabled (ppr_context_type context,
                                           ppr_enabled_status_type *recognition_status);

ppr_error_type ppr_get_num_recognition_threads (ppr_context_type context,
                                                int *num_threads);

ppr_error_type ppr_get_template_extraction_type (ppr_context_type context,
                                                 ppr_template_extraction_type *extraction_type);

ppr_error_type ppr_are_template_thumbnails_enabled (ppr_context_type context,
                                                    ppr_enabled_status_type *template_thumbnail_status);

/*****************************************************************************
 * Gallery Building and Editing Functions
 *****************************************************************************/

ppr_error_type ppr_create_gallery (ppr_context_type context,
                                   ppr_gallery_type *gallery);

ppr_error_type ppr_is_object_suitable_for_recognition (ppr_context_type context,
                                                       ppr_object_type object,
                                                       ppr_object_suitability_type *suitability);

ppr_error_type ppr_extract_template_from_object (ppr_context_type context,
                                                 ppr_image_type image,
                                                 ppr_object_type object,
                                                 ppr_template_type *the_template);

ppr_error_type ppr_copy_template_to_gallery (ppr_context_type context,
                                             ppr_gallery_type *gallery,
                                             ppr_template_type the_template,
                                             int *template_id);

ppr_error_type ppr_remove_template_from_gallery (ppr_context_type context,
                                                 ppr_gallery_type *gallery,
                                                 int template_id);

ppr_error_type ppr_set_template_relationship (ppr_context_type context,
                                              ppr_gallery_type *gallery,
                                              int template_id_a,
                                              int template_id_b,
                                              ppr_template_relationship_type relationship);

ppr_error_type ppr_clear_template_relationships (ppr_context_type context,
                                                 ppr_gallery_type *gallery);

ppr_error_type ppr_set_template_relationships_from_subject_list (ppr_context_type context,
                                                                 ppr_gallery_type *gallery,
                                                                 ppr_subject_list_type subject_list,
                                                                 ppr_template_relationship_type inter_subject_relationship);

ppr_error_type ppr_set_template_string (ppr_context_type context,
                                        ppr_template_type *the_template,
                                        const char *string);

ppr_error_type ppr_set_template_string_by_id (ppr_context_type context,
                                              ppr_gallery_type *gallery,
                                              int template_id,
                                              const char *string);

ppr_error_type ppr_set_gallery_string (ppr_context_type context,
                                       ppr_gallery_type *gallery,
                                       const char *string);
  
/*****************************************************************************
 * Template Query Functions
 *****************************************************************************/

ppr_error_type ppr_get_template_string_length (ppr_context_type context,
                                               ppr_template_type the_template,
                                               size_t *string_length);

ppr_error_type ppr_get_template_string (ppr_context_type context,
                                        ppr_template_type the_template,
                                        char *string);

ppr_error_type ppr_get_template_thumbnail (ppr_context_type context,
                                           ppr_template_type the_template,
                                           ppr_raw_image_type *thumbnail);
  
/*****************************************************************************
 * Gallery Query Functions
 *****************************************************************************/

ppr_error_type ppr_get_num_templates_in_gallery (ppr_context_type context,
                                                 ppr_gallery_type gallery,
                                                 int *num_templates);

ppr_error_type ppr_get_template_id_list_from_gallery (ppr_context_type context,
                                                      ppr_gallery_type gallery,
                                                      ppr_template_id_list_type *template_id_list);

ppr_error_type ppr_get_template_by_id (ppr_context_type context,
                                       ppr_gallery_type gallery,
                                       int template_id,
                                       ppr_template_type *the_template);

ppr_error_type ppr_get_template_string_length_by_id (ppr_context_type context,
                                                     ppr_gallery_type gallery,
                                                     int template_id,
                                                     size_t *string_length);

ppr_error_type ppr_get_template_string_by_id (ppr_context_type context,
                                              ppr_gallery_type gallery,
                                              int template_id,
                                              char *string);

ppr_error_type ppr_get_template_thumbnail_by_id (ppr_context_type context,
                                                 ppr_gallery_type gallery,
                                                 int template_id,
                                                 ppr_raw_image_type *thumbnail);

ppr_error_type ppr_get_gallery_string_length (ppr_context_type context,
                                              ppr_gallery_type gallery,
                                              size_t *string_length);

ppr_error_type ppr_get_gallery_string (ppr_context_type context,
                                       ppr_gallery_type gallery,
                                       char *string);

ppr_error_type ppr_get_user_specified_template_relationship (ppr_context_type context,
                                                             ppr_gallery_type gallery,
                                                             int template_id_a,
                                                             int template_id_b,
                                                             ppr_template_relationship_type *relationship);

ppr_error_type ppr_get_self_similarity_matrix (ppr_context_type context,
                                               ppr_gallery_type gallery,
                                               ppr_similarity_matrix_type *self_similarity_matrix);

/*****************************************************************************
 * Similarity Matrix Query Functions
 *****************************************************************************/

ppr_error_type ppr_get_similarity_matrix_dimensions (ppr_context_type context,
                                                     ppr_similarity_matrix_type similarity_matrix,
                                                     int *num_query_rows,
                                                     int *num_target_cols);

ppr_error_type ppr_get_similarity_matrix_element (ppr_context_type context,
                                                  ppr_similarity_matrix_type similarity_matrix,
                                                  int query_row_id,
                                                  int target_col_id,
                                                  float *score);

ppr_error_type ppr_get_query_template_id_list_from_similarity_matrix (ppr_context_type context,
                                                                      ppr_similarity_matrix_type similarity_matrix,
                                                                      ppr_template_id_list_type *query_template_id_list);

ppr_error_type ppr_get_target_template_id_list_from_similarity_matrix (ppr_context_type context,
                                                                       ppr_similarity_matrix_type similarity_matrix,
                                                                       ppr_template_id_list_type *target_template_id_list);

/************************************************************************************
 * Comparison Functions
 ************************************************************************************/

ppr_error_type ppr_compare_template_to_gallery (ppr_context_type context,
                                                ppr_template_type query_template,
                                                ppr_gallery_type target_gallery,
                                                ppr_score_list_type *score_list);

ppr_error_type ppr_compare_galleries (ppr_context_type context,
                                      ppr_gallery_type query_gallery,
                                      ppr_gallery_type target_gallery,
                                      ppr_similarity_matrix_type *similarity_matrix);

ppr_error_type ppr_recompare_galleries (ppr_context_type context,
                                        ppr_similarity_matrix_type previous_similarity_matrix,
                                        ppr_similarity_matrix_type *new_similarity_matrix);

ppr_error_type ppr_apply_transitive_matching (ppr_context_type context,
                                              ppr_similarity_matrix_type input_similarity_matrix,
                                              ppr_similarity_matrix_type *adjusted_similarity_matrix);

ppr_error_type ppr_trim_subject_to_representative_templates (ppr_context_type context,
                                                             ppr_gallery_type *subject,
                                                             int num_representative_templates);
                                                             
ppr_error_type ppr_get_ranked_template_list_for_template (ppr_context_type context,
                                                          ppr_similarity_matrix_type similarity_matrix,
                                                          int query_template_id,
                                                          int max_list_length,
                                                          float threshold,
                                                          ppr_template_id_list_type *ranked_id_list,
                                                          ppr_score_list_type *scores);

ppr_error_type ppr_get_ranked_template_list_for_subject (ppr_context_type context,
                                                         ppr_similarity_matrix_type similarity_matrix,
                                                         ppr_template_id_list_type query_subject,
                                                         int max_list_length,
                                                         float threshold,
                                                         ppr_template_id_list_type *ranked_id_list,
                                                         ppr_score_list_type *scores);

ppr_error_type ppr_get_ranked_subject_list_for_subject (ppr_context_type context,
                                                        ppr_similarity_matrix_type similarity_matrix,
                                                        ppr_template_id_list_type query_subject,
                                                        ppr_subject_list_type target_subject_list,
                                                        int max_list_length,
                                                        float threshold,
                                                        ppr_index_list_type *ranked_target_indices,
                                                        ppr_score_list_type *scores);

ppr_error_type ppr_get_best_matching_templates (ppr_context_type context,
                                                ppr_similarity_matrix_type similarity_matrix,
                                                ppr_template_id_list_type query_template_ids,
                                                int queries_are_unique,
                                                ppr_template_id_list_type *best_target_ids,
                                                ppr_score_list_type *scores);

ppr_error_type ppr_get_best_matching_subjects (ppr_context_type context,
                                               ppr_similarity_matrix_type similarity_matrix,
                                               ppr_subject_list_type query_subject_list,
                                               ppr_subject_list_type target_subject_list,
                                               int queries_are_unique,
                                               ppr_index_list_type *best_target_indices,
                                               ppr_score_list_type *scores);

/*****************************************************************************
 * File I/O Functions
 *****************************************************************************/

ppr_error_type ppr_write_template (ppr_context_type context,
                                   const char *file_name,
                                   ppr_template_type the_template);

ppr_error_type ppr_read_template (ppr_context_type context,
                                  const char *file_name,
                                  ppr_template_type *the_template);

ppr_error_type ppr_write_gallery (ppr_context_type context,
                                  const char *file_name,
                                  ppr_gallery_type gallery);

ppr_error_type ppr_read_gallery (ppr_context_type context,
                                 const char *file_name,
                                 ppr_gallery_type *gallery);

ppr_error_type ppr_write_gallery_with_subject_list (ppr_context_type context,
                                                    const char *file_name,
                                                    ppr_gallery_type gallery,
                                                    ppr_subject_list_type subject_list);

ppr_error_type ppr_read_gallery_with_subject_list (ppr_context_type context,
                                                   const char *file_name,
                                                   ppr_gallery_type *gallery,
                                                   ppr_subject_list_type *subject_list);

ppr_error_type ppr_write_similarity_matrix (ppr_context_type context,
                                            const char *file_name,
                                            ppr_similarity_matrix_type similarity_matrix);

ppr_error_type ppr_read_similarity_matrix (ppr_context_type context,
                                           const char *file_name,
                                           ppr_similarity_matrix_type *similarity_matrix);

/*****************************************************************************
 * Flatten / Unflatten Functions
 *****************************************************************************/

ppr_error_type ppr_flatten_template (ppr_context_type context,
                                     ppr_template_type the_template,
                                     ppr_flat_template_type *flat_template);

ppr_error_type ppr_unflatten_template (ppr_context_type context,
                                       ppr_flat_template_type flat_template,
                                       ppr_template_type *the_template);

ppr_error_type ppr_flatten_gallery (ppr_context_type context,
                                    ppr_gallery_type gallery,
                                    ppr_flat_gallery_type *flat_gallery);

ppr_error_type ppr_unflatten_gallery (ppr_context_type context,
                                      ppr_flat_gallery_type flat_gallery,
                                      ppr_gallery_type *gallery);

ppr_error_type ppr_flatten_gallery_with_subject_list (ppr_context_type context,
                                                      ppr_gallery_type gallery,
                                                      ppr_subject_list_type subject_list,
                                                      ppr_flat_gallery_type *flat_gallery);

ppr_error_type ppr_unflatten_gallery_with_subject_list (ppr_context_type context,
                                                        ppr_flat_gallery_type flat_gallery,
                                                        ppr_gallery_type *gallery,
                                                        ppr_subject_list_type *subject_list);

ppr_error_type ppr_flatten_similarity_matrix (ppr_context_type context,
                                              ppr_similarity_matrix_type similarity_matrix,
                                              ppr_flat_similarity_matrix_type *flat_similarity_matrix);

ppr_error_type ppr_unflatten_similarity_matrix (ppr_context_type context,
                                                ppr_flat_similarity_matrix_type flat_similarity_matrix,
                                                ppr_similarity_matrix_type *similarity_matrix);

/*****************************************************************************
 * Free Functions
 *****************************************************************************/

void ppr_free_template (ppr_template_type the_template);

void ppr_free_gallery (ppr_gallery_type gallery);

void ppr_free_similarity_matrix (ppr_similarity_matrix_type similarity_matrix);
  
void ppr_free_flat_template (ppr_flat_template_type flat_template);

void ppr_free_flat_gallery (ppr_flat_gallery_type flat_gallery);

void ppr_free_flat_similarity_matrix (ppr_flat_similarity_matrix_type flat_similarity_matrix);

void ppr_free_template_id_list (ppr_template_id_list_type template_id_list);

void ppr_free_index_list (ppr_index_list_type index_list);

void ppr_free_score_list (ppr_score_list_type score_list);

void ppr_free_subject_list (ppr_subject_list_type subject_list);

/*****************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* __PITTPATT_RECOGNITION_H */
