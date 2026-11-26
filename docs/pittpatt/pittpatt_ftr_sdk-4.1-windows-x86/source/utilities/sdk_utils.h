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
 * File: sdk_utils.h
 *
 * This file provides utility functions that are not part of the PittPatt SDK,
 * but can be useful when writing programs that use the SDK.
 *
 * The sdk_utils_read_file_list() function reads in a file list from disk and
 * converts it to an array of strings.  The file list being read must have one
 * file per line (any blank lines are ignored), e.g:
 *
 * ../sample_data/images/still_images/image_01.jpg
 * ../sample_data/images/still_images/image_02.jpg
 * ../sample_data/images/still_images/image_03.jpg
 * ../sample_data/images/still_images/image_04.jpg
 * <EOF>
 *
 *
 * The sdk_utils_free_file_list() function should be used to free data
 * allocated by the sdk_utils_read_file_list() function.
 *
 * The filename manipulation functions use the following naming convention:
 *
 * |---------path----------|
 * |                       |
 * |         |--filename---|
 * |         |             |
 * directory/title.extension
 * |             |
 * |--basename---|
 *
 */

#ifndef __SDK_UTILS_H
#define __SDK_UTILS_H

#include <stdio.h>

#include "pittpatt_ftr_sdk.h"

#include "pittpatt_raw_image_io.h"
#include "pittpatt_video_io.h"

#include "sdk_draw_utils.h"

/* If C++ then we need to __extern "C". Compiler defines __cplusplus */
#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Constants
 *****************************************************************************/

#define SDK_UTILS_INDENT_STR_LEN 32
#define FUNCTION_NAME_MAX        256

/*****************************************************************************
 * Macros
 *****************************************************************************/

#define PRINT_FUNCTION_NAME(CC)                                         \
  {                                                                     \
    char _function_name[FUNCTION_NAME_MAX];                             \
    int _i;                                                             \
                                                                        \
    _i = 0;                                                             \
    while (#CC[_i] != ' ' && #CC[_i] != '(')                            \
    {                                                                   \
      _function_name[_i] = #CC[_i];                                     \
      _i++;                                                             \
    }                                                                   \
    _function_name[_i] = '\0';                                          \
    fprintf(stderr, "%s()", _function_name);                            \
  }

#define TRY(CC)                                                         \
  {                                                                     \
    ppr_error_type _err = (CC);                                         \
                                                                        \
    if (_err != PPR_SUCCESS)                                            \
    {                                                                   \
      if (_err != PPR_REQUIRES_FRONTAL_FACE_OBJECT)                     \
      {                                                                 \
        fprintf(stderr, "ERROR - %s: ", ppr_error_message(_err));       \
        PRINT_FUNCTION_NAME(CC)                                         \
        fprintf(stderr, " (%s:%d)\n", __FILE__, __LINE__);              \
        ppr_finalize_sdk();                                             \
        exit(EXIT_FAILURE);                                             \
      }                                                                 \
      else                                                              \
      {                                                                 \
        fprintf(stderr, "%s: ", ppr_error_message(_err));               \
        PRINT_FUNCTION_NAME(CC)                                         \
        fprintf(stderr, " (%s:%d)\n", __FILE__, __LINE__);              \
      }                                                                 \
    }                                                                   \
  }

#define TRY_RAW_IMAGE(CC)                                               \
  {                                                                     \
    ppr_raw_image_error_type _err = (CC);                               \
    if (_err != PPR_RAW_IMAGE_SUCCESS)                                  \
    {                                                                   \
      fprintf(stderr, "ERROR - %s: ", ppr_raw_image_error_message(_err)); \
      PRINT_FUNCTION_NAME(CC)                                           \
      fprintf(stderr, " (%s:%d)\n", __FILE__, __LINE__);                \
      ppr_finalize_sdk();                                               \
      exit(EXIT_FAILURE);                                               \
    }                                                                   \
  }

#define TRY_VIDEO(CC)                                                   \
  {                                                                     \
    ppr_video_io_error_type _err = (CC);                                \
    if (_err != PPR_VIDEO_IO_SUCCESS)                                   \
    {                                                                   \
      fprintf(stderr, "ERROR - %s: ", ppr_video_io_error_message(_err)); \
      PRINT_FUNCTION_NAME(CC)                                           \
      fprintf(stderr, " (%s:%d)\n", __FILE__, __LINE__);                \
      ppr_finalize_sdk();                                               \
      exit(EXIT_FAILURE);                                               \
    }                                                                   \
  }

/*****************************************************************************
 * Data Types
 *****************************************************************************/

typedef struct
{
  char **filenames; /* Array of filenames           */
  int size;         /* Number of filenames in array */
} sdk_utils_file_list_type;

typedef struct
{
  int num_colors;
  sdk_draw_utils_color_type *colors;
  int *ids;
  int capacity;
} sdk_utils_id_color_list_type;

/*****************************************************************************
 * Print Functions
 *****************************************************************************/

/* Create string containing the specified number of spaces */
void sdk_utils_get_indent_str(
  int indent,
  char *indent_str);

/* Prints an id color list to the display */
void sdk_utils_id_color_list_print(
  sdk_utils_id_color_list_type id_color_list);

/*****************************************************************************
 * Color Functions
 *****************************************************************************/

/* Allocates the internal buffers of an id color list and sets the number of colors to 0 */
void sdk_utils_id_color_list_allocate(
  sdk_utils_id_color_list_type *id_color_list,
  int max_num_colors);

/* Adds an entry to an id color list, exiting with an error if the capacity is exceeded */
void sdk_utils_id_color_list_append(
  sdk_utils_id_color_list_type *id_color_list,
  int id,
  sdk_draw_utils_color_type color);

/* Returns the color associated with an id in the id color list */
sdk_draw_utils_color_type sdk_utils_id_color_list_get_color(
  sdk_utils_id_color_list_type id_color_list,
  int id);

/* Frees the internal buffers of an id color list */
void sdk_utils_id_color_list_free(
  sdk_utils_id_color_list_type id_color_list);

/*****************************************************************************
 * Overlay Functions
 *****************************************************************************/

/* Overlays bounding box and landmarks from a single object onto an image and puts a string under the bounding box */
void sdk_utils_overlay_object(
  ppr_raw_image_type *raw_image,
  ppr_object_type object,
  const char *text,
  sdk_draw_utils_line_attributes_type line_attributes);

/* Overlays bounding boxes and landmarks from all objects onto an image */
void sdk_utils_overlay_object_list(
  ppr_raw_image_type *raw_image,
  ppr_object_list_type object_list,
  int check_confidence);

/* Overlays bounding boxes and landmarks from all tracked objects that meet the minimum confidence */
void sdk_utils_overlay_tracked_object_list(
  ppr_raw_image_type *raw_image,
  ppr_tracked_object_list_type tracked_object_list,
  int min_confidence);

/* Overlays a frame number in the upper-left corner of an image */
void sdk_utils_overlay_frame_number(
  ppr_raw_image_type *raw_image,
  int frame_num);
    
/* Advance tracked object indices until frame_num is reached or passed */
void sdk_utils_advance_indices_to_frame(
  ppr_completed_track_list_type completed_track_list,
  int *i_tracked_object_lists,
  int frame_num);

/* Overlays bounding boxes and landmarks from completed tracks onto a copy of the images in the list  */
void sdk_utils_overlay_completed_track_list_on_image_list(
  sdk_utils_file_list_type in_image_list,
  ppr_completed_track_list_type completed_track_list,
  int min_confidence,
  int overlay_frame_numbers,
  const char *output_image_format);

/* Overlays bounding boxes and landmarks from completed tracks onto copies of the frames in the video */
void sdk_utils_overlay_completed_track_list_on_video(
  const char *in_filename,
  ppr_completed_track_list_type completed_track_list,
  int min_confidence,
  int overlay_frame_numbers,
  const char *output_image_format);

/*****************************************************************************
 * String Conversion Functions
 *****************************************************************************/

/* Returns a string for the ppr_enabled_status_string_type */
const char *sdk_utils_get_enabled_status_string(
  ppr_enabled_status_type status);

/*****************************************************************************
 * File List Functions
 *****************************************************************************/

/* Returns 1 if the path specifies a directory, 0 otherwise */
int sdk_utils_is_directory(
  const char *path);

/* Gets a list of files from a directory (does not include directories) */
void sdk_utils_get_file_list_from_directory(
  const char* parent_directory,
  sdk_utils_file_list_type *file_list);

/* Gets a list of directories from a directory (does not include files) */
void sdk_utils_get_directory_list_from_directory(
  const char* parent_directory,
  sdk_utils_file_list_type *directory_list);

/* Loads a file list from a file to an array of strings */
void sdk_utils_read_file_list(
  const char* filename,
  sdk_utils_file_list_type *file_list);

/* Frees memory used to hold file list components */
void sdk_utils_free_file_list(
  sdk_utils_file_list_type file_list);

/*****************************************************************************
 * Filename Manipulation Functions
 *****************************************************************************/

/* Get directory from a string */
void sdk_utils_get_file_directory(
  const char *input,
  char *directory);

/* Get title from a string */
void sdk_utils_get_file_title(
  const char *input,
  char *title);

/* Get extension from a string */
void sdk_utils_get_file_extension(
  const char *input,
  char *extension);

/* Get basename from a string */
void sdk_utils_get_file_basename(
  const char *input,
  char *basename);

/* Get filename from a string */
void sdk_utils_get_file_filename(
  const char *input,
  char *filename);

/*****************************************************************************
 * Error Checking Functions
 *****************************************************************************/

/* Prints message if err is anything other than PPR_SUCCESS, exits if fatal */ 
void sdk_utils_check_ppr_error(
  ppr_error_type err,
  const char *function_name);

/* Prints message and exits if err is not PPR_RAW_IMAGE_SUCCESS */ 
void sdk_utils_check_ppr_raw_image_error(
  ppr_raw_image_error_type err,
  const char *function_name);

/* Prints message and exits if err is not PPR_VIDEO_IO_SUCCESS */ 
void sdk_utils_check_ppr_video_io_error(
  ppr_video_io_error_type err,
  const char *function_name);

/* Prints message and exits if input is NULL */ 
void sdk_utils_check_allocation(
  void *input,
  const char *input_name,
  const char *calling_function_name);

/* Prints message and exits if file descriptor is NULL */ 
void sdk_utils_check_file_open(
  FILE *fid,
  const char *file_name,
  const char *calling_function_name);

/*****************************************************************************
 * Miscellaneous Functions
 *****************************************************************************/

/* Returns the number of CPUs in the system */
int sdk_utils_get_num_cpus(void);

/* Returns the minimum of two integers */
int sdk_utils_get_min_int(
  int a,
  int b);

/* Sorts a float array in descending order and returns indices */
void sdk_utils_sort_float_array_descending(
  float *array,
  int n_elements,
  int *indices);

/* Returns indices of sorted float array, but does not alter input */
void sdk_utils_get_float_array_descending_indices(
  const float *in_array,
  int n_elements,
  int *indices);

/* Creates a directory */
void sdk_utils_make_directory(
  const char *directory_name);

/* Print the context configuration */
void sdk_utils_print_context_configuration(
  FILE *stream,
  ppr_context_type context);


/*****************************************************************************
 * Detection & Tracking Print Functions
 *****************************************************************************/

/* Prints the contents of a ppr_polygon_type structure */
void sdk_utils_print_polygon(
  FILE *stream,
  ppr_polygon_type polygon,
  int indent);

/* Prints the contents of a ppr_landmark_type structure */
void sdk_utils_print_landmark(
  FILE *stream,
  ppr_landmark_type landmark,
  int indent);

/* Prints a list of landmarks */
void sdk_utils_print_landmarks(
  FILE *stream,
  int num_landmarks,
  ppr_landmark_type *landmarks,
  int indent,
  int verbose);

/* Prints the contents of a ppr_object_type structure */
void sdk_utils_print_object(
  FILE *stream,
  ppr_object_type object,
  int indent,
  int verbose);

/* Prints the contents of a ppr_object_list_type structure */
void sdk_utils_print_object_list(
  FILE *stream,
  ppr_object_list_type object_list,
  int indent,
  int verbose);

/* Prints the contents of a ppr_tracked_object_type structure */
void sdk_utils_print_tracked_object(
  FILE *stream,
  ppr_tracked_object_type tracked_object,
  int indent,
  int verbose);

/* Prints a list of tracked objects */
void sdk_utils_print_tracked_objects(
  FILE *stream,
  int num_tracked_objects,
  ppr_tracked_object_type *tracked_objects,
  int indent,
  int verbose);

/* Prints the contents of a ppr_tracked_object_list_type structure */
void sdk_utils_print_tracked_object_list(
  FILE *stream,
  ppr_tracked_object_list_type tracked_object_list,
  int indent,
  int verbose);

/* Prints the contents of a ppr_track_summary_type structure */
void sdk_utils_print_track_summary(
  FILE *stream,
  ppr_track_summary_type track_summary,
  int indent);
  
/* Prints the contents of a ppr_completed_track_type structure */
void sdk_utils_print_completed_track(
  FILE *stream,
  ppr_completed_track_type completed_track,
  int print_objects_in_track,
  int indent,
  int verbose);

/* Prints the contents of a ppr_completed_track_list_type structure */
void sdk_utils_print_completed_track_list(
  FILE *stream,
  ppr_completed_track_list_type completed_track_list,
  int print_objects_in_track,
  int indent,
  int verbose);

/*****************************************************************************
 * Detection & Tracking String Conversion Functions
 *****************************************************************************/

/* Returns a string for the ppr_landmark_category_type */
const char *sdk_utils_get_landmark_category_string(
  ppr_landmark_category_type landmark_category);

/* Returns a string for the ppr_precision_type */
const char* sdk_utils_get_precision_string(
  ppr_precision_type precision);

/* Returns a string for the ppr_landmark_detector_type */
const char* sdk_utils_get_landmark_detector_string(
  ppr_landmark_detector_type landmark_detector);

/* Returns the ppr_precision_type specified by a string */
int sdk_utils_get_precision_from_string(
  const char *precision_str,
  ppr_precision_type *precision);

/* Returns the ppr_landmark_detector_type specified by a string */
int sdk_utils_get_landmark_detector_from_string(
  const char *landmark_detector_str,
  ppr_landmark_detector_type *landmark_detector);

/* Returns the search region coordinates specified by a string */
int sdk_utils_get_search_region_from_string(
  const char *search_region_str,
  int *x1,
  int *y1,
  int *x2,
  int *y2);

/*****************************************************************************
 * Miscellaneous Detection & Tracking Functions
 *****************************************************************************/

/* Write all visual instances of all completed tracks to specified directory */
void sdk_utils_write_visual_instances(
  const char *directory,
  const char *output_image_format,
  ppr_completed_track_list_type completed_track_list);

/*****************************************************************************
 * Recognition Print Functions
 *****************************************************************************/

/* Prints the contents of a ppr_template_id_list_type structure */
void sdk_utils_print_template_id_list(
  FILE *stream,
  ppr_template_id_list_type template_id_list,
  int verbose);

/* Prints the contents of a ppr_index_list_type structure */
void sdk_utils_print_index_list(
  FILE *stream,
  ppr_index_list_type index_list,
  int verbose);

/* Prints the contents of a ppr_subject_list_type structure */
void sdk_utils_print_subject_list(
  FILE *stream,
  ppr_subject_list_type subject_list,
  int verbose);

/* Prints the contents of a ppr_similarity_matrix_type structure */
void sdk_utils_print_similarity_matrix(
  FILE *stream,
  ppr_context_type context,
  ppr_similarity_matrix_type similarity_matrix,
  int verbose);

/* Prints the contents of a ppr_gallery_type structure */
void sdk_utils_print_gallery(
  FILE *stream,
  ppr_context_type context,
  ppr_gallery_type gallery,
  int verbose);

/* Prints the contents of a ppr_inter_gallery_subject_list_type structure */
void sdk_utils_print_inter_gallery_subject_list(
  FILE *stream,
  ppr_inter_gallery_subject_list_type subject_list,
  int verbose);

/*****************************************************************************
 * Recognition String Conversion Functions
 *****************************************************************************/

/* Returns a string for the ppr_frontal_yaw_constraint_type */
const char* sdk_utils_get_frontal_yaw_constraint_string(
  ppr_frontal_yaw_constraint_type frontal_yaw_constraint);

/* Returns the ppr_frontal_yaw_constraint_type specified by a string */
int sdk_utils_get_frontal_yaw_constraint_from_string(
  const char *frontal_yaw_constraint_str,
  ppr_frontal_yaw_constraint_type *frontal_yaw_constraint);

/* Returns a string for the ppr_template_extraction_type */
const char* sdk_utils_get_template_extraction_type_string(
  ppr_template_extraction_type extraction_type);

/* Returns the ppr_template_extraction_type specified by a string */
int sdk_utils_get_template_extraction_type_from_string(
  const char *extraction_type_str,
  ppr_template_extraction_type *extraction_type);

/*****************************************************************************
 * Miscellanous Recognition Functions
 *****************************************************************************/

/* Writes template thumbnail to the specified file.  Writes gray image if template has no thumbnail */
void sdk_utils_write_template_thumbnail(
  const char *file_name,
  ppr_context_type context,
  ppr_template_type the_template);

/* Writes thumbnail of template ID to the specified file.  Writes gray image if template has no thumbnail */
void sdk_utils_write_template_thumbnail_by_id(
  const char *file_name,
  ppr_context_type context,
  ppr_gallery_type gallery,
  int template_id);

/* Writes thumbnails of all templates in a template id list to a directory */
void sdk_utils_write_template_id_list_thumbnails(
  const char *directory,
  ppr_context_type context,
  ppr_gallery_type gallery,
  int write_source_images,
  ppr_template_id_list_type template_id_list);

/* Writes a directory structure of thumbnails where each directory is a single subject */
void sdk_utils_write_subject_list_thumbnails(
  const char *directory,
  ppr_context_type context,
  ppr_gallery_type gallery,
  int write_source_images,
  ppr_subject_list_type subject_list);

/* Adds an item to a template string, marking the item with the specifed marker string */
void sdk_utils_add_item_to_template_string(
  char *template_string,
  const char *marker_string,
  char *item_string);

/* Gets an item from a template string where the item is marked by the specifed marker string */
void sdk_utils_get_item_from_template_string(
  const char *template_string,
  const char *marker_string,
  char *item_string);

#ifdef __cplusplus
}
#endif

#endif /* __SDK_UTILS_H */
