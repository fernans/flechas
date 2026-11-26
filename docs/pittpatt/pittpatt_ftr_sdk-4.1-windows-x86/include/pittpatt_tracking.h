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


#ifndef __PITTPATT_TRACKING_H
#define __PITTPATT_TRACKING_H

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

#define PPR_TRACKING_CUTOFF_MIN       -5  /* Most permissive  */
#define PPR_TRACKING_CUTOFF_MAX        5  /* Most restrictive */

#define PPR_MAX_NUM_VISUAL_INSTANCES  10

/*****************************************************************************
 * Structure Types
 *****************************************************************************/

typedef struct
{
  int index;
  double time_stamp;
  int tracked_id;
  int track_confidence;
  ppr_object_type object;
} ppr_tracked_object_type;

typedef struct
{
  int num_tracked_objects;
  ppr_tracked_object_type *tracked_objects;
} ppr_tracked_object_list_type;

typedef struct
{
  size_t num_bytes;
  void *data;
} ppr_flat_tracked_object_list_type;

typedef struct
{
  int tracked_id;
  int start_index;
  int stop_index;
  ppr_coordinate_type start_position;
  ppr_coordinate_type stop_position;
  ppr_dimensions_type min_dimensions;
  ppr_dimensions_type max_dimensions;
  ppr_dimensions_type avg_dimensions;
  float min_confidence;
  float max_confidence;
  float avg_confidence;
} ppr_track_summary_type;

typedef struct
{
  int index;
  ppr_raw_image_type image;
} ppr_visual_instance_type;

typedef struct 
{
  ppr_track_summary_type track_summary;
  int num_tracked_objects;
  ppr_tracked_object_type *tracked_objects;
  int num_visual_instances;
  ppr_visual_instance_type *visual_instances;
} ppr_completed_track_type;

typedef struct
{
  int num_completed_tracks;
  ppr_completed_track_type *completed_tracks;
} ppr_completed_track_list_type;

typedef struct
{
  size_t num_bytes;
  void *data;
} ppr_flat_completed_track_list_type;

/*****************************************************************************
 * Initialize and Configure Functions
 *****************************************************************************/

ppr_error_type ppr_enable_tracking (ppr_context_type context);

ppr_error_type ppr_set_tracking_cutoff (ppr_context_type context,
                                        int tracking_cutoff);

ppr_error_type ppr_enable_visual_attention (ppr_context_type context);

ppr_error_type ppr_set_num_visual_instances (ppr_context_type context,
                                             int num_visual_instances);

ppr_error_type ppr_enable_shot_boundary_detection (ppr_context_type context);

ppr_error_type ppr_set_shot_boundary_threshold (ppr_context_type context,
                                                float threshold);

/*****************************************************************************
 * Context Query Functions
 *****************************************************************************/

ppr_error_type ppr_is_tracking_enabled (ppr_context_type context,
                                        ppr_enabled_status_type *tracking_status);

ppr_error_type ppr_get_tracking_cutoff (ppr_context_type context,
                                        int *tracking_cutoff);

ppr_error_type ppr_is_visual_attention_enabled (ppr_context_type context,
                                                ppr_enabled_status_type *visual_attention_status);

ppr_error_type ppr_get_num_visual_instances (ppr_context_type context,
                                             int *num_visual_instances);

ppr_error_type ppr_is_shot_boundary_detection_enabled (ppr_context_type context,
                                                       ppr_enabled_status_type *shot_boundary_status);

ppr_error_type ppr_get_shot_boundary_threshold (ppr_context_type context,
                                                float *threshold);

/*****************************************************************************
 * Track Functions
 *****************************************************************************/

ppr_error_type ppr_track_objects (ppr_context_type context,
                                  ppr_image_type image,
                                  ppr_object_list_type object_list);

ppr_error_type ppr_detect_and_track_objects (ppr_context_type context,
                                             ppr_image_type image);

/*****************************************************************************
 * Results Functions
 *****************************************************************************/

ppr_error_type ppr_get_current_frame_objects (ppr_context_type context,
                                              ppr_tracked_object_list_type *tracked_object_list);

ppr_error_type ppr_get_completed_tracks (ppr_context_type context,
                                         ppr_completed_track_list_type *completed_track_list);

ppr_error_type ppr_finalize_current_tracks (ppr_context_type context);

ppr_error_type ppr_discard_unreturned_completed_tracks (ppr_context_type context);

ppr_error_type ppr_get_dispersed_frontal_objects_from_track (ppr_context_type context,
                                                             ppr_completed_track_type completed_track,
                                                             int max_num_objects,
                                                             ppr_tracked_object_list_type *frontal_objects_list);

ppr_error_type ppr_reset_tracker (ppr_context_type context);

/*****************************************************************************
 * File I/O Functions
 *****************************************************************************/

ppr_error_type ppr_write_tracked_object_list (ppr_context_type context,
                                              const char *file_name,
                                              ppr_tracked_object_list_type tracked_object_list,
                                              const char *source_file_name);

ppr_error_type ppr_read_tracked_object_list (ppr_context_type context,
                                             const char *file_name,
                                             ppr_tracked_object_list_type *tracked_object_list,
                                             char *source_file_name);

ppr_error_type ppr_write_completed_track_list (ppr_context_type context,
                                               const char *file_name,
                                               ppr_completed_track_list_type completed_track_list,
                                               const char *source_file_name);

ppr_error_type ppr_read_completed_track_list (ppr_context_type context,
                                              const char *file_name,
                                              ppr_completed_track_list_type *completed_track_list,
                                              char *source_file_name);

/*****************************************************************************
 * Flatten / Unflatten Functions
 *****************************************************************************/

ppr_error_type ppr_flatten_tracked_object_list (ppr_context_type context,
                                                ppr_tracked_object_list_type tracked_object_list,
                                                ppr_flat_tracked_object_list_type *flat_tracked_object_list,
                                                const char *source_file_name);

ppr_error_type ppr_unflatten_tracked_object_list (ppr_context_type context,
                                                  ppr_flat_tracked_object_list_type flat_tracked_object_list,
                                                  ppr_tracked_object_list_type *tracked_object_list,
                                                  char *source_file_name);

ppr_error_type ppr_flatten_completed_track_list (ppr_context_type context,
                                                 ppr_completed_track_list_type completed_track_list,
                                                 ppr_flat_completed_track_list_type *flat_completed_track_list,
                                                 const char *source_file_name);

ppr_error_type ppr_unflatten_completed_track_list (ppr_context_type context,
                                                   ppr_flat_completed_track_list_type flat_completed_track_list,
                                                   ppr_completed_track_list_type *completed_track_list,
                                                   char *source_file_name);

/*****************************************************************************
 * Free Functions
 *****************************************************************************/

void ppr_free_tracked_object_list (ppr_tracked_object_list_type tracked_object_list);

void ppr_free_flat_tracked_object_list (ppr_flat_tracked_object_list_type flat_tracked_object_list);
  
void ppr_free_completed_track_list (ppr_completed_track_list_type completed_track_list);

void ppr_free_flat_completed_track_list (ppr_flat_completed_track_list_type flat_completed_track_list);

/*****************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* __PITTPATT_TRACKING_H */
