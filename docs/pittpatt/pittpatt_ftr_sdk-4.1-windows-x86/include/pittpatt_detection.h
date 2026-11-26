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


#ifndef __PITTPATT_DETECTION_H
#define __PITTPATT_DETECTION_H

#include <stdio.h>

#include "pittpatt_raw_image.h"
#include "pittpatt_errors.h"

/* If C++ then we need to extern "C". Compiler defines __cplusplus */
#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Constants
 *****************************************************************************/

#define PPR_MIN_MIN_SIZE                       -2
#define PPR_MAX_MAX_SIZE                        100

#define PPR_MANUAL_LANDMARKS                    0
#define PPR_AUTOMATIC_LANDMARKS                 1

#define PPR_MAX_SEARCH_PRUNING_AGGRESSIVENESS   5
#define PPR_MAX_SOURCE_FILE_NAME_SIZE           1024

/*****************************************************************************
 * Basic Types
 *****************************************************************************/

typedef struct ppr_context_struct* ppr_context_type;
typedef struct ppr_image_struct* ppr_image_type;

/*****************************************************************************
 * Enumerated Types
 *****************************************************************************/

typedef enum
{
  PPR_FRONTAL_DETECTOR_MASK            = 0x01,
  PPR_RIGHT_PROFILE_DETECTOR_MASK      = 0x02,
  PPR_LEFT_PROFILE_DETECTOR_MASK       = 0x04,
  PPR_FRONTAL_TILTED_N30_DETECTOR_MASK = 0x08,
  PPR_FRONTAL_TILTED_P30_DETECTOR_MASK = 0x10,
  PPR_ALL_FACE_DETECTORS_MASK          = 0x1F
} ppr_coarse_detector_mask_type;

typedef enum
{
  PPR_NO_LANDMARK_DETECTOR,
  PPR_FRONTAL_LANDMARK_DETECTOR,
  PPR_MULTI_POSE_LANDMARK_DETECTOR,
  PPR_DUAL_FRONTAL_LANDMARK_DETECTOR,
  PPR_DUAL_MULTI_POSE_LANDMARK_DETECTOR
} ppr_landmark_detector_type;

typedef enum
{
  PPR_LANDMARK_LEFT_EYE,
  PPR_LANDMARK_RIGHT_EYE,
  PPR_LANDMARK_NOSE_BASE,
  PPR_LANDMARK_NOSE_BRIDGE,
  PPR_LANDMARK_NOSE_TIP,
  PPR_LANDMARK_NOSE_TOP,
  PPR_LANDMARK_EYE_NOSE,
  PPR_LANDMARK_MOUTH
} ppr_landmark_category_type;

typedef enum
{
  PPR_COARSE_PRECISION,
  PPR_FINE_PRECISION
} ppr_precision_type;

typedef enum
{
  PPR_FRONTAL_YAW_CONSTRAINT_RESTRICTIVE,
  PPR_FRONTAL_YAW_CONSTRAINT_PERMISSIVE
} ppr_frontal_yaw_constraint_type;

typedef enum
{
  PPR_DEFAULT_BOUNDING_BOX,
  PPR_TIGHT_BOUNDING_BOX
} ppr_bounding_box_type;

typedef enum
{
  PPR_DISABLED,
  PPR_ENABLED
} ppr_enabled_status_type;

/*****************************************************************************
 * Structure Types
 *****************************************************************************/

typedef struct
{
  float x;
  float y;
} ppr_coordinate_type;

typedef struct
{
  int num_points;
  ppr_coordinate_type *points;
} ppr_polygon_type;

typedef struct
{
  int model_id;
  ppr_landmark_category_type category;
  ppr_coordinate_type position;
} ppr_landmark_type;

typedef struct
{
  float width;
  float height;
} ppr_dimensions_type;

typedef struct
{
  float yaw;
  float pitch;
  float roll;
  ppr_precision_type precision;
} ppr_rotation_type;

typedef struct
{
  int model_id;
  ppr_coordinate_type position;
  ppr_dimensions_type dimensions;
  ppr_rotation_type rotation;
  float size;
  float confidence;
  int num_landmarks;
  ppr_landmark_type *landmarks;
} ppr_object_type;

typedef struct
{
  int num_objects;
  ppr_object_type *objects;
} ppr_object_list_type;

typedef struct
{
  size_t num_bytes;
  void *data;
} ppr_flat_object_list_type;

/*****************************************************************************
 * Initialize and Configure Functions
 *****************************************************************************/

ppr_context_type ppr_get_context (void);

ppr_error_type ppr_set_license (ppr_context_type context,
                                const char *license_id, 
                                const unsigned int *license_key);

ppr_error_type ppr_set_models_path (ppr_context_type context,
                                    const char *path);

ppr_error_type ppr_set_detection_precision (ppr_context_type context,
                                            ppr_precision_type precision);

ppr_error_type ppr_set_coarse_detector_types (ppr_context_type context,
                                              ppr_coarse_detector_mask_type masks);

ppr_error_type ppr_set_landmark_detector_type (ppr_context_type context,
                                               ppr_landmark_detector_type landmark_detector,
                                               int automatic_setting);

ppr_error_type ppr_set_frontal_yaw_constraint (ppr_context_type context,
                                               ppr_frontal_yaw_constraint_type yaw_constraint);

ppr_error_type ppr_set_num_detection_threads (ppr_context_type context,
                                              int num_threads);

ppr_error_type ppr_set_search_pruning_aggressiveness (ppr_context_type context,
                                                      int aggressiveness);

ppr_error_type ppr_set_min_size (ppr_context_type context,
                                 int min_size);

ppr_error_type ppr_set_max_size (ppr_context_type context,
                                 int max_size);

ppr_error_type ppr_set_adaptive_min_size (ppr_context_type context,
                                          float adaptive_min_size);

ppr_error_type ppr_set_adaptive_max_size (ppr_context_type context,
                                          float adaptive_max_size);

ppr_error_type ppr_set_detection_threshold (ppr_context_type context,
                                            float threshold);

ppr_error_type ppr_set_search_region (ppr_context_type context,
                                      int x1,
                                      int y1,
                                      int x2,
                                      int y2);

ppr_error_type ppr_clear_search_region (ppr_context_type context);

ppr_error_type ppr_initialize_context (ppr_context_type context);

/*****************************************************************************
 * Context Query Functions 
 *****************************************************************************/

ppr_error_type ppr_get_models_path (ppr_context_type context,
                                    const char **path);

ppr_error_type ppr_get_detection_precision (ppr_context_type context,
                                            ppr_precision_type *precision);

ppr_error_type ppr_get_coarse_detector_types (ppr_context_type context,
                                              ppr_coarse_detector_mask_type *masks);

ppr_error_type ppr_get_landmark_detector_type (ppr_context_type context,
                                               ppr_landmark_detector_type *landmark_detector);

ppr_error_type ppr_get_frontal_yaw_constraint (ppr_context_type context,
                                               ppr_frontal_yaw_constraint_type *yaw_constraint);

ppr_error_type ppr_get_num_detection_threads (ppr_context_type context,
                                              int *num_threads);

ppr_error_type ppr_get_search_pruning_aggressiveness (ppr_context_type context,
                                                      int *aggressiveness);

ppr_error_type ppr_get_min_size (ppr_context_type context,
                                 int *min_size);

ppr_error_type ppr_get_max_size (ppr_context_type context,
                                 int *max_size);

ppr_error_type ppr_get_adaptive_min_size (ppr_context_type context,
                                          float *adaptive_min_size);

ppr_error_type ppr_get_adaptive_max_size (ppr_context_type context,
                                          float *adaptive_max_size);

ppr_error_type ppr_get_detection_threshold (ppr_context_type context,
                                            float *threshold);

ppr_error_type ppr_get_search_region (ppr_context_type context,
                                      int *x1,
                                      int *y1,
                                      int *x2,
                                      int *y2);

/*****************************************************************************
 * Miscellaneous Query Functions 
 *****************************************************************************/

ppr_error_type ppr_get_size_from_pixels (int pixels,
                                         float *size);

/*****************************************************************************
 * Detect Functions
 *****************************************************************************/

ppr_error_type ppr_create_image (ppr_raw_image_type raw_image,
                                 ppr_image_type *image);

ppr_error_type ppr_detect_objects (ppr_context_type context,
                                   ppr_image_type image,
                                   ppr_object_list_type *object_list);

ppr_error_type ppr_detect_best_frontal_object (ppr_context_type context,
                                               ppr_image_type image,
                                               ppr_object_list_type *object_list);

ppr_error_type ppr_detect_landmarks_from_object (ppr_context_type context,
                                                 ppr_image_type image,
                                                 ppr_object_type *object);

/*****************************************************************************
 * Miscellaneous Functions
 *****************************************************************************/

ppr_error_type ppr_get_object_boundary (ppr_object_type object,
                                        ppr_bounding_box_type bounding_box,
                                        ppr_polygon_type *polygon);

ppr_error_type ppr_crop_object_from_image (ppr_context_type context,
                                           ppr_image_type image,
                                           ppr_object_type object,
                                           ppr_raw_image_type *cropped_image);

ppr_error_type ppr_crop_thumbnail_from_image (ppr_context_type context,
                                              ppr_image_type image,
                                              ppr_object_type object,
                                              ppr_raw_image_type *thumbnail);

const char* ppr_error_message (ppr_error_type error_code);

ppr_error_type ppr_write_image (ppr_image_type image,
                                const char *base_file_name);

const char* ppr_version (void);

/*****************************************************************************
 * File I/O Functions
 *****************************************************************************/

ppr_error_type ppr_write_object_list (ppr_context_type context,
                                      const char *file_name,
                                      ppr_object_list_type object_list,
                                      const char *source_image_name);

ppr_error_type ppr_read_object_list (ppr_context_type context,
                                     const char *file_name,
                                     ppr_object_list_type *object_list,
                                     char *source_image_name);

/*****************************************************************************
 * Flatten / Unflatten Functions
 *****************************************************************************/

ppr_error_type ppr_flatten_object_list (ppr_context_type context,
                                        ppr_object_list_type object_list,
                                        ppr_flat_object_list_type *flat_object_list,
                                        const char *source_image_name);

ppr_error_type ppr_unflatten_object_list (ppr_context_type context,
                                          ppr_flat_object_list_type flat_object_list_type,
                                          ppr_object_list_type *object_list,
                                          char *source_image_name);

/*****************************************************************************
 * Free Functions
 *****************************************************************************/

void ppr_free_object_list (ppr_object_list_type object_list);

void ppr_free_flat_object_list (ppr_flat_object_list_type flat_object_list);

void ppr_free_polygon (ppr_polygon_type polygon);

void ppr_free_image (ppr_image_type image);

/*****************************************************************************
 * Finalize Functions
 *****************************************************************************/

void ppr_finalize_thread (void);

ppr_error_type ppr_release_context (ppr_context_type context);

void ppr_finalize_sdk (void);

/*****************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* __PITTPATT_DETECTION_H */
