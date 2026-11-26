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
 * File: sdk_utils.c
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

#if !defined (WIN32) && (defined (_WIN32) || defined (__WIN32) || defined (_WIN32_) || defined (__WIN32__))
#define WIN32
#endif

#if !defined (linux) && (defined (__linux) || defined (__linux__) || defined (__gnu_linux__))
#define linux
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>

#ifdef linux
/* required for S_IFDIR from sys/stat.h on older version of linux */
#define __USE_BSD
#endif

#include <sys/stat.h>
#if defined (WIN32)
#include <direct.h>
#else
#include <dirent.h>
#endif

/* For get_num_cpus() */
#if defined (WIN32)
#include <windows.h>
#elif defined (linux)
#include <sys/sysinfo.h>
#elif defined (__MACH__)
#include <sys/sysctl.h>
#endif

#include "pittpatt_ftr_sdk.h"

#include "pittpatt_raw_image_io.h"
#include "pittpatt_video_io.h"

#include "sdk_utils.h"
#include "sdk_draw_utils.h"

/*****************************************************************************
 * Constants
 *****************************************************************************/

#define LOW_CONFIDENCE_COLOR                    SDK_DRAW_UTILS_BLUE
#define LOW_CONFIDENCE_THICKNESS                2

#define MEDIUM_CONFIDENCE_THRESHOLD             0.5f
#define MEDIUM_CONFIDENCE_COLOR                 SDK_DRAW_UTILS_YELLOW
#define MEDIUM_CONFIDENCE_THICKNESS             3

#define HIGH_CONFIDENCE_THRESHOLD               1.5f
#define HIGH_CONFIDENCE_COLOR                   SDK_DRAW_UTILS_GREEN
#define HIGH_CONFIDENCE_THICKNESS               4

#define CROSS_SIZE                              6
#define CROSS_THICKNESS                         1

#define MAX_COMMAND_LENGTH                    512

#define SDK_UTILS_NUM_ENABLED_STATUS_STRINGS    2

#define BIT_RATE                              1000

/*****************************************************************************
 * Static Variables
 *****************************************************************************/

static const char *sdk_utils_static_enabled_status_strings[] = 
{
  "disabled",
  "enabled"
};

/*******************************************************************************
 * Data Types
 ******************************************************************************/

typedef enum
{
  SDK_UTILS_DIRECTORY_ITEM_ALL,
  SDK_UTILS_DIRECTORY_ITEM_DIRECTORIES_ONLY,
  SDK_UTILS_DIRECTORY_ITEM_FILES_ONLY
} sdk_utils_directory_item_type;

/*****************************************************************************
 * Local Function Prototypes
 *****************************************************************************/

static void sdk_utils_overlay_objects(
  ppr_raw_image_type *raw_image,
  int num_objects,
  ppr_object_type *objects,
  int check_confidence);

static void sdk_utils_overlay_tracked_objects(
  ppr_raw_image_type *raw_image,
  int num_tracked_objects,
  ppr_tracked_object_type *tracked_objects,
  int min_confidence);

static void sdk_utils_get_string_position_below_polygon(
  ppr_polygon_type polygon,
  const char *str,
  int scale,
  sdk_draw_utils_point_type *position);

static char *sdk_utils_get_last_slash_ptr(
  const char *input);

static void sdk_utils_get_num_items_in_directory(
  const char *parent_directory,
  int *num_items,
  sdk_utils_directory_item_type item_type);

static void sdk_utils_get_item_list_from_directory(
  const char* parent_directory,
  sdk_utils_file_list_type *item_list,
  sdk_utils_directory_item_type item_type);

/*****************************************************************************
 * Print Functions
 *****************************************************************************/

/* Create string containing the specified number of spaces */
void sdk_utils_get_indent_str(
  int indent,
  char *indent_str)
{
  int i;
  int n_spaces;

  indent_str[0] = '\0';

  if (indent >= SDK_UTILS_INDENT_STR_LEN)
    n_spaces = SDK_UTILS_INDENT_STR_LEN - 1;
  else
    n_spaces = indent;

  for (i = 0; i < n_spaces; i++)
    strcat(indent_str, " ");
}

/******************************************************************************/

/* Prints an id color list to the display */
void sdk_utils_id_color_list_print(
  sdk_utils_id_color_list_type id_color_list)
{
  int i;

  printf("Number of colors: %d\n", id_color_list.num_colors);
  printf("Capacity: %d\n", id_color_list.capacity);

  for (i = 0; i < id_color_list.num_colors; i++)
    printf("%d: id = %d, color = %s\n", i, id_color_list.ids[i],
           sdk_draw_utils_get_color_string(id_color_list.colors[i]));
}

/*****************************************************************************
 * Color Functions
 *****************************************************************************/

/* Allocates the internal buffers of an id color list and sets the number of colors to 0 */
void sdk_utils_id_color_list_allocate(
  sdk_utils_id_color_list_type *id_color_list,
  int max_num_colors)
{
  id_color_list->num_colors = 0;

  id_color_list->colors = (sdk_draw_utils_color_type*)malloc(max_num_colors * sizeof(sdk_draw_utils_color_type));
  sdk_utils_check_allocation(id_color_list->colors, "id_color_list->colors", __FUNCTION__);

  id_color_list->ids = (int*)malloc(max_num_colors * sizeof(int));
  sdk_utils_check_allocation(id_color_list->ids, "id_color_list->ids", __FUNCTION__);

  id_color_list->capacity = max_num_colors;
}

/******************************************************************************/

/* Adds an entry to an id color list, exiting with an error if the capacity is exceeded */
void sdk_utils_id_color_list_append(
  sdk_utils_id_color_list_type *id_color_list,
  int id,
  sdk_draw_utils_color_type color)
{
  if (id_color_list->num_colors == id_color_list->capacity)
  {
    fprintf(stderr, "ERROR: attempted to exceed capacity of id color list\n");
    exit(EXIT_FAILURE);
  }

  id_color_list->colors[id_color_list->num_colors] = color;
  id_color_list->ids[id_color_list->num_colors] = id;

  id_color_list->num_colors++;
}

/******************************************************************************/

/* Returns the color associated with an id in the id color list */
sdk_draw_utils_color_type sdk_utils_id_color_list_get_color(
  sdk_utils_id_color_list_type id_color_list,
  int id)
{
  int i;

  for (i = 0; i < id_color_list.num_colors; i++)
  {
    if (id_color_list.ids[i] == id)
      return id_color_list.colors[i];
  }

  return (sdk_draw_utils_color_type)-1;
}

/******************************************************************************/

/* Frees the internal buffers of an id color list */
void sdk_utils_id_color_list_free(
  sdk_utils_id_color_list_type id_color_list)
{
  free(id_color_list.colors);
  free(id_color_list.ids);
}

/*****************************************************************************
 * Overlay Functions
 *****************************************************************************/

/* Overlays bounding box and landmarks from a single object onto an image and puts a string under the bounding box */
void sdk_utils_overlay_object(
  ppr_raw_image_type *raw_image,
  ppr_object_type object,
  const char *text,
  sdk_draw_utils_line_attributes_type line_attributes)
{
  int landmark_num;

  ppr_polygon_type polygon;
  sdk_draw_utils_text_attributes_type text_attributes;
  sdk_draw_utils_point_type string_top_left;

  sdk_draw_utils_init_text_attributes(&text_attributes);
  text_attributes.color = SDK_DRAW_UTILS_WHITE;
  text_attributes.background_color = SDK_DRAW_UTILS_BLACK;
  
  /* If color space is GRAY8, covert to RGB24 to get color overlays */
  if (raw_image->color_space == PPR_RAW_IMAGE_GRAY8)
    ppr_raw_image_convert(raw_image, PPR_RAW_IMAGE_RGB24);

  ppr_get_object_boundary(object, PPR_DEFAULT_BOUNDING_BOX, &polygon);

  sdk_utils_get_string_position_below_polygon(polygon, text, text_attributes.scale, &string_top_left);
  sdk_draw_utils_draw_polygon(raw_image, polygon, line_attributes);
  sdk_draw_utils_draw_string(raw_image, text, string_top_left, text_attributes);
  ppr_free_polygon(polygon);

  line_attributes.line_type = SDK_DRAW_UTILS_SOLID_LINE;
  line_attributes.thickness = CROSS_THICKNESS;
  
  for (landmark_num = 0; landmark_num < object.num_landmarks; landmark_num++)
    sdk_draw_utils_draw_cross(raw_image, object.landmarks[landmark_num].position, CROSS_SIZE, line_attributes);
}

/*****************************************************************************/

/* Overlays bounding boxes and landmarks from all objects onto an image */
void sdk_utils_overlay_object_list(
  ppr_raw_image_type *raw_image,
  ppr_object_list_type object_list,
  int check_confidence)
{
  sdk_utils_overlay_objects(raw_image, object_list.num_objects, object_list.objects, check_confidence);
}

/*****************************************************************************/

/* Overlays bounding boxes and landmarks from all tracked objects that meet the minimum confidence */
void sdk_utils_overlay_tracked_object_list(
  ppr_raw_image_type *raw_image,
  ppr_tracked_object_list_type tracked_object_list,
  int min_confidence)
{
  sdk_utils_overlay_tracked_objects(raw_image, tracked_object_list.num_tracked_objects, tracked_object_list.tracked_objects, min_confidence);
}

/******************************************************************************/

/* Overlays a frame number in the upper-left corner of an image */
void sdk_utils_overlay_frame_number(
  ppr_raw_image_type *raw_image,
  int frame_num)
{
  char frame_number_string[32];
  sdk_draw_utils_point_type string_top_left;
  sdk_draw_utils_text_attributes_type text_attributes;

  sdk_draw_utils_init_text_attributes(&text_attributes);
  text_attributes.color = SDK_DRAW_UTILS_WHITE;
  text_attributes.background_color = SDK_DRAW_UTILS_BLACK;
  text_attributes.scale = 2;
  
  string_top_left.x = 0;
  string_top_left.y = 0;
  
  sprintf(frame_number_string, "%d", frame_num);
  sdk_draw_utils_draw_string(raw_image, frame_number_string, string_top_left, text_attributes);
}

/******************************************************************************/

/* Advance tracked object indices until frame_num is reached or passed */
void sdk_utils_advance_indices_to_frame(
  ppr_completed_track_list_type completed_track_list,
  int *i_tracked_object_lists,
  int frame_num)
{
  int i;

  for (i = 0; i < completed_track_list.num_completed_tracks; i++)
  {
    while (
      (i_tracked_object_lists[i] < completed_track_list.completed_tracks[i].num_tracked_objects - 1) &&
      (completed_track_list.completed_tracks[i].tracked_objects[i_tracked_object_lists[i]].index < frame_num))
    {
      i_tracked_object_lists[i]++;
    }
  }
}

/*****************************************************************************/

/* Overlays bounding boxes and landmarks from completed tracks onto a copy of the images in the list  */
void sdk_utils_overlay_completed_track_list_on_image_list(
  sdk_utils_file_list_type in_image_list,
  ppr_completed_track_list_type completed_track_list,
  int min_confidence,
  int overlay_frame_numbers,
  const char *output_image_format)
{
  int i;
  int frame_num;

  int *i_tracked_object_lists;

  ppr_raw_image_type raw_image;
  char title[FILENAME_MAX];
  char out_filename[FILENAME_MAX];
  
  /* Create array of indices into tracked object arrays */
  i_tracked_object_lists = (int*)malloc(completed_track_list.num_completed_tracks * sizeof(int));
  sdk_utils_check_allocation(i_tracked_object_lists, "i_tracked_object_lists", __FUNCTION__);
  for (i = 0; i < completed_track_list.num_completed_tracks; i++)
    i_tracked_object_lists[i] = 0;

  /* Loop through each image in list */
  for (frame_num = 0; frame_num < in_image_list.size; frame_num++)
  {
    /* Read image */
    TRY_RAW_IMAGE(ppr_raw_image_io_read(in_image_list.filenames[frame_num], &raw_image));

    /* Overlay polygons on frame */
    sdk_utils_advance_indices_to_frame(completed_track_list, i_tracked_object_lists, frame_num);
    for (i = 0; i < completed_track_list.num_completed_tracks; i++)
    {
      if (completed_track_list.completed_tracks[i].tracked_objects[i_tracked_object_lists[i]].index == frame_num)
      {
        /* Overlay polygon for this object */
        sdk_utils_overlay_tracked_objects(&raw_image, 1, &completed_track_list.completed_tracks[i].tracked_objects[i_tracked_object_lists[i]], min_confidence);
      }
    }

    /* Overlay frame number on frame */
    if (overlay_frame_numbers)
      sdk_utils_overlay_frame_number(&raw_image, frame_num);

    /* Write image to file */
    sdk_utils_get_file_title(in_image_list.filenames[frame_num], title);
    sprintf(out_filename, "%s_completed_track_overlay.%s", title, output_image_format);
    TRY_RAW_IMAGE(ppr_raw_image_io_write(out_filename, raw_image));

    /* Free image */
    ppr_raw_image_free(raw_image);
  }

  /* Clean up */
  free(i_tracked_object_lists);
}

/*****************************************************************************/

/* Overlays bounding boxes and landmarks from completed tracks onto copies of the frames in the video */
void sdk_utils_overlay_completed_track_list_on_video(
  const char *in_filename,
  ppr_completed_track_list_type completed_track_list,
  int min_confidence,
  int overlay_frame_numbers,
  const char *output_video_format)
{
  int i;
  int frame_num;

  ppr_video_io_type input_video;
  ppr_video_io_type output_video;
  int frame_rate_numerator;
  int frame_rate_denominator;
  int *i_tracked_object_lists;

  ppr_raw_image_type raw_image;
  char title[FILENAME_MAX];
  char out_filename[FILENAME_MAX];
  
  /* Open video */
  TRY_VIDEO(ppr_video_io_open(&input_video, in_filename));
  TRY_VIDEO(ppr_video_io_get_frame_rate(input_video, &frame_rate_numerator, &frame_rate_denominator));

  /* Create output video */
  sdk_utils_get_file_title(in_filename, title);
  sprintf(out_filename, "%s_completed_track_overlay.%s", title, output_video_format);
  TRY_VIDEO(ppr_video_io_create(&output_video, out_filename, BIT_RATE, frame_rate_numerator, frame_rate_denominator));

  /* Create array of indices into tracked object arrays */
  i_tracked_object_lists = (int*)malloc(completed_track_list.num_completed_tracks * sizeof(int));
  sdk_utils_check_allocation(i_tracked_object_lists, "i_tracked_object_lists", __FUNCTION__);
  for (i = 0; i < completed_track_list.num_completed_tracks; i++)
    i_tracked_object_lists[i] = 0;

  /* Loop through each frame of video */
  frame_num = 0;
  while (ppr_video_io_is_frame_available(input_video))
  {
    /* Read frame from video */
    TRY_VIDEO(ppr_video_io_get_frame(input_video, &raw_image));

    /* Overlay polygons on frame */
    sdk_utils_advance_indices_to_frame(completed_track_list, i_tracked_object_lists, frame_num);
    for (i = 0; i < completed_track_list.num_completed_tracks; i++)
    {
      if (completed_track_list.completed_tracks[i].tracked_objects[i_tracked_object_lists[i]].index == frame_num)
      {
        /* Overlay polygon for this object */
        sdk_utils_overlay_tracked_objects(&raw_image, 1, &completed_track_list.completed_tracks[i].tracked_objects[i_tracked_object_lists[i]], min_confidence);
      }
    }

    /* Overlay frame number on frame */
    if (overlay_frame_numbers)
      sdk_utils_overlay_frame_number(&raw_image, frame_num);

    /* Write frame to output video */
    TRY_VIDEO(ppr_video_io_add_frame(output_video, raw_image));

    /* Free frame */
    ppr_raw_image_free(raw_image);

    /* Advance to next frame */
    TRY_VIDEO(ppr_video_io_step_forward(input_video));
    frame_num++;
  }

  /* Close video */
  TRY_VIDEO(ppr_video_io_close(input_video));
  TRY_VIDEO(ppr_video_io_close(output_video));

  /* Clean up */
  free(i_tracked_object_lists);
}

/*****************************************************************************
 * String Conversion Functions
 *****************************************************************************/

/* Returns a string for the ppr_enabled_status_type */
const char *sdk_utils_get_enabled_status_string(
  ppr_enabled_status_type status)
{
  if (status < 0 || status >= SDK_UTILS_NUM_ENABLED_STATUS_STRINGS)
    return "";

  return sdk_utils_static_enabled_status_strings[status];
}

/*****************************************************************************
 * File List Functions
 *****************************************************************************/

/* Returns 1 if the path specifies a directory, 0 otherwise */
int sdk_utils_is_directory(
  const char *path)
{
#ifdef WIN32

  struct _stat stat_buf;
  _stat(path, &stat_buf);

#else

  struct stat stat_buf;
  stat(path, &stat_buf);

#endif

  if (stat_buf.st_mode & S_IFDIR)
    return 1;
  
  return 0;
}

/******************************************************************************/

/* Gets a list of files from a directory (does not include directories) */
void sdk_utils_get_file_list_from_directory(
  const char* parent_directory,
  sdk_utils_file_list_type *file_list)
{
  sdk_utils_get_item_list_from_directory(parent_directory, file_list, SDK_UTILS_DIRECTORY_ITEM_FILES_ONLY);
}

/******************************************************************************/

/* Gets a list of directories from a directory (does not include files) */
void sdk_utils_get_directory_list_from_directory(
  const char* parent_directory,
  sdk_utils_file_list_type *directory_list)
{
  sdk_utils_get_item_list_from_directory(parent_directory, directory_list, SDK_UTILS_DIRECTORY_ITEM_DIRECTORIES_ONLY);
}

/******************************************************************************/

/* Loads a file list from a file to an array of strings */
void sdk_utils_read_file_list(
  const char* filename,
  sdk_utils_file_list_type *file_list)
{
  char c = '\0';
  int i, row, j;

  int rowcount = 0;
  int size = 0;
  int prev;

  FILE *filepointer;
  char *file_array;
  
  /* Open filelist, make sure it exists. */
  filepointer = fopen(filename, "rt");
  sdk_utils_check_file_open(filepointer, filename, __FUNCTION__);

  /* Find length of file */
  while (c != EOF)
  {
    c = (char)getc(filepointer);
    size++;
  }  

  /* Read file */
  file_array = (char*)malloc(size);
  sdk_utils_check_allocation(file_array, "file_array", __FUNCTION__);

  rewind(filepointer);  
  for (i = 0; i < size; i++)
    file_array[i] = (char)getc(filepointer);

  if (file_array[size - 1] != EOF)
  {
    fprintf(stderr, "ERROR: EOF not found at end of file in file_list_open()\n");
    ppr_finalize_sdk();
    exit(EXIT_FAILURE);
  }

  /* Change EOF to \n to ensure it acts as an ending seperator */
  file_array[size - 1] = '\n';  
  
  prev = 0;
  i = 0;
  
  /* Skip initial blank lines */
  while (file_array[i] == '\n' && i < size)
    i++;
  
  /* Count valid lines */
  for ( ; i < size; i++)      
  {
    if (file_array[i] == '\n')
    {
      /* Ignore consecutive end-of-lines (blank lines) */
      if (i - prev > 1)
        rowcount++;
      prev = i;
    }
  }
  
  file_list->filenames = (char**)malloc(rowcount * sizeof(char*));
  sdk_utils_check_allocation(file_list->filenames, "file_list->filenames", __FUNCTION__);

  file_list->size = rowcount;
  prev = 0;
  i = 0;
  
  /* Break filelist into array of strings. */
  for (row = 0; row < file_list->size; row++)
  {
    i = prev;
    c = file_array[i];
    
    /* Find next newline */
    while (c != '\n')
      c = file_array[++i]; 

    /* Copy current filename */
    file_list->filenames[row] = (char*)malloc(i - prev + 1);  
    sdk_utils_check_allocation(file_list->filenames[row], "file_list->filenames[row]", __FUNCTION__);

    for (j = 0; j < i - prev; j++)
      file_list->filenames[row][j] = file_array[prev + j];
    
    /* Terminate copied string */
    file_list->filenames[row][i - prev] = '\0';         
    
    /* Prepare for next iteration */
    prev = i;
    
    /* Skip blank lines */
    if (row < file_list->size - 1)
    {
      while (file_array[prev] == '\n')
        prev++;
    }
  }
  
  free(file_array);
  fclose(filepointer);
}

/*****************************************************************************/

/* Frees memory used to hold file list components */
void sdk_utils_free_file_list(
  sdk_utils_file_list_type file_list)
{
  int i;

  for (i = 0; i < file_list.size; i++)
    free(file_list.filenames[i]);

  free(file_list.filenames);
}

/*****************************************************************************
 * Filename Manipulation Functions
 *****************************************************************************/

/* Get directory from a string */
void sdk_utils_get_file_directory(
  const char *input,
  char *directory)
{
  const char *last_slash_ptr;
  int directory_len;

  last_slash_ptr = sdk_utils_get_last_slash_ptr(input);

  if (last_slash_ptr == NULL)
  {
    directory[0] = '\0';
  }
  else
  {
    directory_len = (int)(last_slash_ptr - input);
    strncpy(directory, input, directory_len);
    directory[directory_len] = '\0';
  }
}

/*****************************************************************************/

/* Get title from a string */
void sdk_utils_get_file_title(
  const char *input,
  char *title)
{
  const char *last_dot_ptr;
  const char *last_slash_ptr;
  const char *begin_ptr;
  const char *end_ptr;
  int title_len;

  last_dot_ptr = strrchr(input, '.');
  
  last_slash_ptr = sdk_utils_get_last_slash_ptr(input);

  if (last_dot_ptr == NULL || last_dot_ptr < last_slash_ptr)
    end_ptr = input + strlen(input) - 1;
  else
    end_ptr = last_dot_ptr - 1;

  if (last_slash_ptr == NULL)
    begin_ptr = input;
  else
    begin_ptr = last_slash_ptr + 1;

  title_len = (int)(end_ptr - begin_ptr + 1);
  strncpy(title, begin_ptr, title_len);
  title[title_len] = '\0';
}

/*****************************************************************************/

/* Get extension from a string */
void sdk_utils_get_file_extension(
  const char *input,
  char *extension)
{
  const char *last_dot_ptr;
  const char *last_slash_ptr;

  last_dot_ptr = strrchr(input, '.');
  
  last_slash_ptr = sdk_utils_get_last_slash_ptr(input);

  if (last_dot_ptr == NULL || last_dot_ptr < last_slash_ptr)
    extension[0] = '\0';
  else
    strcpy(extension, last_dot_ptr + 1);
}

/*****************************************************************************/

/* Get basename from a string */
void sdk_utils_get_file_basename(
  const char *input,
  char *basename)
{
  const char *last_dot_ptr;
  int basename_len;

  last_dot_ptr = strrchr(input, '.');

  if (last_dot_ptr == NULL)
  {
    strcpy(basename, input);
  }
  else
  {
    basename_len = (int)(last_dot_ptr - input);
    strncpy(basename, input, basename_len);
    basename[basename_len] = '\0';
  }
}

/*****************************************************************************/

/* Get filename from a string */
void sdk_utils_get_file_filename(
  const char *input,
  char *filename)
{
  const char *last_slash_ptr;

  last_slash_ptr = sdk_utils_get_last_slash_ptr(input);

  if (last_slash_ptr == NULL)
    strcpy(filename, input);
  else
    strcpy(filename, last_slash_ptr + 1);
}

/*****************************************************************************
 * Error Checking Functions
 *****************************************************************************/

/* Prints message if err is anything other than PPR_SUCCESS, exits if fatal */ 
void sdk_utils_check_ppr_error(
  ppr_error_type err,
  const char *function_name)
{
  if (err != PPR_SUCCESS)
  {
    if (err != PPR_REQUIRES_FRONTAL_FACE_OBJECT)
    {
      fprintf(stderr, "ERROR returned from %s(): %s\n", function_name, ppr_error_message(err));
      ppr_finalize_sdk();
      exit(EXIT_FAILURE);
    }
    else
    {
      fprintf(stderr, "%s(): %s\n", function_name, ppr_error_message(err));
    }
  }
}

/*****************************************************************************/

/* Prints message and exits if err is not PPR_RAW_IMAGE_SUCCESS */ 
void sdk_utils_check_ppr_raw_image_error(
  ppr_raw_image_error_type err,
  const char *function_name)
{
  if (err != PPR_RAW_IMAGE_SUCCESS)
  {
    fprintf(stderr, "ERROR returned from %s(): %s\n", function_name, ppr_raw_image_error_message(err));
    ppr_finalize_sdk();
    exit(EXIT_FAILURE);
  }
}

/*****************************************************************************/

/* Prints message and exits if err is not PPR_VIDEO_IO_SUCCESS */ 
void sdk_utils_check_ppr_video_io_error(
  ppr_video_io_error_type err,
  const char *function_name)
{
  if (err != PPR_VIDEO_IO_SUCCESS)
  {
    fprintf(stderr, "ERROR returned from %s(): %s\n", function_name, ppr_video_io_error_message(err));
    ppr_finalize_sdk();
    exit(EXIT_FAILURE);
  }
}

/*****************************************************************************/

/* Prints message and exits if input is NULL */ 
void sdk_utils_check_allocation(
  void *input,
  const char *input_name,
  const char *calling_function_name)
{
  if (input == NULL)
  {
    fprintf(stderr, "ERROR: could not allocate %s in %s()\n", input_name, calling_function_name);
    ppr_finalize_sdk();
    exit(EXIT_FAILURE);
  }
}

/*****************************************************************************/

/* Prints message and exits if file descriptor is NULL */ 
void sdk_utils_check_file_open(
  FILE *fid,
  const char *file_name,
  const char *calling_function_name)
{
  if (fid == NULL)
  {
    fprintf(stderr, "ERROR: could not open %s in %s()\n", file_name, calling_function_name);
    ppr_finalize_sdk();
    exit(EXIT_FAILURE);
  }
}

/*****************************************************************************
 * Miscellaneous Functions
 *****************************************************************************/

/* Returns the number of CPUs in the system */
int sdk_utils_get_num_cpus(void)
{
  int ncpu = 0;

#if defined (WIN32)
  SYSTEM_INFO sysInfo;
  GetSystemInfo (&sysInfo);
  ncpu = sysInfo.dwNumberOfProcessors;
#elif defined (linux)
  ncpu = get_nprocs();
#elif defined (__MACH__)
  size_t size = sizeof(int);
  sysctlbyname("hw.ncpu", &ncpu, &size, NULL, (size_t)0);
#else
  ncpu = 1;
#endif

  if (ncpu < 2)
    ncpu = 1;

  return ncpu;
}

/*****************************************************************************/

/* Returns the minimum of two integers */
int sdk_utils_get_min_int(
  int a,
  int b)
{
  if (a < b)
    return a;
  else
    return b;
}

/*****************************************************************************/

/* Sorts a float array in descending order and returns indices */
void sdk_utils_sort_float_array_descending(
  float *in_array,
  int n_elements,
  int *indices)
{
  int i;         /* Loop index                              */
  int j;         /* Loop index                              */
  float max;     /* Maximum element                         */
  int index_max; /* Index of maximum element                */
  int temp;      /* Temporary variable for swapping indices */

  /* Initialize indices array */
  for (i = 0; i < n_elements; i++)
    indices[i] = i;

  /* Loop through all elements */
  for (i = 0; i < n_elements; i++)
  {
    /* Initialize max to current element */
    max = in_array[i];
    index_max = i;

    /* Loop through remaining elements */
    for (j = i + 1; j < n_elements; j++)
    {
      /* Set this element to max if greater than current max */
      if (in_array[j] > max)
      {
        max = in_array[j];
        index_max = j;
      }
    }
    
    /* Swap current element with max */
    in_array[index_max] = in_array[i];
    in_array[i] = max;

    temp = indices[index_max];
    indices[index_max] = indices[i];
    indices[i] = temp;
  }
}

/*****************************************************************************/

/* Returns indices of sorted float array, but does not alter input */
void sdk_utils_get_float_array_descending_indices(
  const float *in_array,
  int n_elements,
  int *indices)
{
  int i;
  float *copy;

  /* Make copy of array so it is not affected by sort */
  copy = (float*)malloc(sizeof(float) * n_elements);
  sdk_utils_check_allocation(copy, "copy", __FUNCTION__);
  for (i = 0; i < n_elements; i++)
    copy[i] = in_array[i];

  /* Sort array, getting indices */
  sdk_utils_sort_float_array_descending(copy, n_elements, indices);

  free(copy);
}

/*****************************************************************************/

/* Creates a directory */
void sdk_utils_make_directory(
  const char *directory_name)
{
  int err;

#ifdef WIN32
  /* Without permissions argument */
  err = _mkdir(directory_name);
#else
  /* With permissions argument - 0 prefix makes it octal */
  err = mkdir(directory_name, 0777);
#endif

  if (err < 0)
  {
    fprintf(stderr, "ERROR: could not create directory %s\n", directory_name);
    ppr_finalize_sdk();
    exit(EXIT_FAILURE);
  }
}

/*****************************************************************************
 * Local Functions
 *****************************************************************************/

/* Overlays bounding boxes and landmarks from all objects onto an image */
static void sdk_utils_overlay_objects(
  ppr_raw_image_type *raw_image,
  int num_objects,
  ppr_object_type *objects,
  int check_confidence)
{
  int object_num;

  ppr_object_type *curr_object;
  sdk_draw_utils_line_attributes_type line_attributes;

  sdk_draw_utils_init_line_attributes(&line_attributes);
  line_attributes.line_type = SDK_DRAW_UTILS_SOLID_LINE;

  for (object_num = 0; object_num < num_objects; object_num++)
  {
    curr_object = &objects[object_num];

    line_attributes.color = HIGH_CONFIDENCE_COLOR;
    line_attributes.thickness = HIGH_CONFIDENCE_THICKNESS;
    if (check_confidence)
    {
      if (curr_object->confidence < MEDIUM_CONFIDENCE_THRESHOLD)
      {
        line_attributes.color = LOW_CONFIDENCE_COLOR;
        line_attributes.thickness = LOW_CONFIDENCE_THICKNESS;
      }
      else if (curr_object->confidence < HIGH_CONFIDENCE_THRESHOLD)
      {
        line_attributes.color = MEDIUM_CONFIDENCE_COLOR;
        line_attributes.thickness = MEDIUM_CONFIDENCE_THICKNESS;
      }
    }
    
    sdk_utils_overlay_object(raw_image, *curr_object, "", line_attributes);
  }
}

/*****************************************************************************/

/* Overlays bounding boxes and landmarks from all tracked objects that meet the minimum confidence */
static void sdk_utils_overlay_tracked_objects(
  ppr_raw_image_type *raw_image,
  int num_tracked_objects,
  ppr_tracked_object_type *tracked_objects,
  int min_confidence)
{
  int tracked_object_num;
  char id_string[32];
  sdk_draw_utils_line_attributes_type line_attributes;

  sdk_draw_utils_init_line_attributes(&line_attributes);
  line_attributes.color = HIGH_CONFIDENCE_COLOR;
  line_attributes.line_type = SDK_DRAW_UTILS_SOLID_LINE;
  line_attributes.thickness = HIGH_CONFIDENCE_THICKNESS;
  
  for (tracked_object_num = 0; tracked_object_num < num_tracked_objects; tracked_object_num++)
  {
    if (tracked_objects[tracked_object_num].track_confidence >= min_confidence)
    {
      sprintf(id_string, "id=%d", tracked_objects[tracked_object_num].tracked_id);
      sdk_utils_overlay_object(raw_image, tracked_objects[tracked_object_num].object, id_string, line_attributes);
    }
  }
}

/******************************************************************************/

static void sdk_utils_get_string_position_below_polygon(
  ppr_polygon_type polygon,
  const char *str,
  int scale,
  sdk_draw_utils_point_type *position)
{
  int i;
  int cnt;
  ppr_coordinate_type top_left;
  ppr_coordinate_type bottom_right;
  int num_chars;
  int num_pixels;

  top_left.x = FLT_MAX;
  top_left.y = FLT_MAX;
  bottom_right.x = -FLT_MAX;
  bottom_right.y = -FLT_MAX;
  
  /* Get extents of polygon */
  for (i = 0; i < polygon.num_points; i++)
  {
    if (polygon.points[i].x < top_left.x)
      top_left.x = polygon.points[i].x;

    if (polygon.points[i].x > bottom_right.x)
      bottom_right.x = polygon.points[i].x;

    if (polygon.points[i].y < top_left.y)
      top_left.y = polygon.points[i].y;

    if (polygon.points[i].y > bottom_right.y)
      bottom_right.y = polygon.points[i].y;
  }

  /* Get the maximum number of characters on a single line */
  i = 0;
  cnt = 0;
  num_chars = 0;
  while (str[i] != '\0')
  {
    if (str[i] == '\n')
    {
      if (cnt > num_chars)
        num_chars = cnt;
      cnt = 0;
    }
    else
    {
      cnt++;
    }
    
    i++;
  }
  if (cnt > num_chars)
    num_chars = cnt;
  
  num_pixels = num_chars * SDK_DRAW_UTILS_CHAR_WIDTH * scale;

  position->x = (int)((bottom_right.x + top_left.x) / 2.0f);
  position->x -= num_pixels / 2;

  position->y = (int)bottom_right.y + 5;
}

/*****************************************************************************/

/* Get the last slash or backslash in a string */
static char *sdk_utils_get_last_slash_ptr(
  const char *input)
{
  char *last_forwardslash_ptr;
  char *last_backslash_ptr;
  char *last_slash_ptr;

  last_forwardslash_ptr = strrchr(input, '/');
  last_backslash_ptr = strrchr(input, '\\');

  if (last_forwardslash_ptr == NULL)
  {
    last_slash_ptr = last_backslash_ptr;
  }
  else if (last_backslash_ptr == NULL)
  {
    last_slash_ptr = last_forwardslash_ptr;
  }
  else
  {
    if (last_forwardslash_ptr > last_backslash_ptr)
      last_slash_ptr = last_forwardslash_ptr;
    else
      last_slash_ptr = last_backslash_ptr;
  }

  return last_slash_ptr;
}

/*****************************************************************************/

/* Gets the number of items (directories and/or files) in a directory, not including ones that start with a '.' */
static void sdk_utils_get_num_items_in_directory(
  const char *parent_directory,
  int *num_items,
  sdk_utils_directory_item_type item_type)
{
#ifdef WIN32

  HANDLE h;
  WIN32_FIND_DATAA data;
  char filename[FILENAME_MAX];
  int is_directory;
  BOOL items_remain;

  sprintf(filename, "%s\\*", parent_directory);
  h = FindFirstFileA(filename, &data);
  if (h == INVALID_HANDLE_VALUE)
  {
    fprintf(stderr, "ERROR: could not read directory %s in %s()\n", parent_directory, __FUNCTION__);
    ppr_finalize_sdk();
    exit(EXIT_FAILURE);
  }
  
  *num_items = 0;
  items_remain = TRUE;
  while (items_remain)
  {
    if (data.cFileName[0] != '.' && data.cFileName[0] != '\0')
    {
      sprintf(filename, "%s\\%s", parent_directory, data.cFileName);
      is_directory = sdk_utils_is_directory(filename);
      if ((item_type == SDK_UTILS_DIRECTORY_ITEM_ALL) ||
          (item_type == SDK_UTILS_DIRECTORY_ITEM_DIRECTORIES_ONLY && is_directory) ||
          (item_type == SDK_UTILS_DIRECTORY_ITEM_FILES_ONLY && !is_directory))
      {
        (*num_items)++;
      }
    }

    items_remain = FindNextFileA(h, &data);
  }

  if (GetLastError() != ERROR_NO_MORE_FILES)
  {
    fprintf(stderr, "ERROR: could not read directory %s in %s()\n", parent_directory, __FUNCTION__);
    ppr_finalize_sdk();
    exit(EXIT_FAILURE);
  }

#else

  DIR *dir_stream;
  struct dirent *entry;
  char filename[FILENAME_MAX];
  int is_directory;

  dir_stream = opendir(parent_directory);
  if (dir_stream == NULL)
  {
    fprintf(stderr, "ERROR: could not read directory %s in %s()\n", parent_directory, __FUNCTION__);
    ppr_finalize_sdk();
    exit(EXIT_FAILURE);
  }

  *num_items = 0;
  while((entry = readdir(dir_stream)) != NULL)
  {
    if (entry->d_name[0] != '.' && entry->d_name[0] != '\0')
    {
      sprintf(filename, "%s/%s", parent_directory, entry->d_name);
      is_directory = sdk_utils_is_directory(filename);
      if ((item_type == SDK_UTILS_DIRECTORY_ITEM_ALL) ||
          (item_type == SDK_UTILS_DIRECTORY_ITEM_DIRECTORIES_ONLY && is_directory) ||
          (item_type == SDK_UTILS_DIRECTORY_ITEM_FILES_ONLY && !is_directory))
      {
        (*num_items)++;
      }
    }
  }

  closedir(dir_stream);

#endif
}

/******************************************************************************/

/* Gets a list of items (directories and/or files) in a directory */
static void sdk_utils_get_item_list_from_directory(
  const char* parent_directory,
  sdk_utils_file_list_type *item_list,
  sdk_utils_directory_item_type item_type)
{
#ifdef WIN32

  int i;
  HANDLE h;
  WIN32_FIND_DATAA data;
  size_t len;
  char filename[FILENAME_MAX];
  int is_directory;
  BOOL items_remain;

  sdk_utils_get_num_items_in_directory(parent_directory, &item_list->size, item_type);
  if (item_list->size == 0)
	  return;

  sprintf(filename, "%s\\*", parent_directory);
  h = FindFirstFileA(filename, &data);
  if (h == INVALID_HANDLE_VALUE)
  {
    fprintf(stderr, "ERROR: could not read directory %s in %s()\n", parent_directory, __FUNCTION__);
    ppr_finalize_sdk();
    exit(EXIT_FAILURE);
  }

  item_list->filenames = (char**)malloc(item_list->size * sizeof(char*));
  sdk_utils_check_allocation(item_list->filenames, "item_list->filenames", __FUNCTION__);

  i = 0;
  items_remain = TRUE;
  while (items_remain)
  {
    if (data.cFileName[0] != '.' && data.cFileName[0] != '\0')
    {
      sprintf(filename, "%s/%s", parent_directory, data.cFileName);
      is_directory = sdk_utils_is_directory(filename);
      if ((item_type == SDK_UTILS_DIRECTORY_ITEM_ALL) ||
          (item_type == SDK_UTILS_DIRECTORY_ITEM_DIRECTORIES_ONLY && is_directory) ||
          (item_type == SDK_UTILS_DIRECTORY_ITEM_FILES_ONLY && !is_directory))
      {
        len = strlen(data.cFileName);
        item_list->filenames[i] = (char*)malloc((len + 1) * sizeof(char));
        sdk_utils_check_allocation(item_list->filenames[i], "item_list->filenames[i]", __FUNCTION__);
        strcpy(item_list->filenames[i], data.cFileName);
        i++;
      }
    }
    items_remain = FindNextFileA(h, &data);
  }

  if (GetLastError() != ERROR_NO_MORE_FILES)
  {
    fprintf(stderr, "ERROR: could not read directory %s in %s()\n", parent_directory, __FUNCTION__);
    ppr_finalize_sdk();
    exit(EXIT_FAILURE);
  }

#else

  int i;
  
  DIR *dir_stream;
  struct dirent *entry;
  int len;
  char filename[FILENAME_MAX];
  int is_directory;

  sdk_utils_get_num_items_in_directory(parent_directory, &item_list->size, item_type);
  if (item_list->size == 0)
    return;
  
  dir_stream = opendir(parent_directory);
  if (dir_stream == NULL)
  {
    fprintf(stderr, "ERROR: could not read directory %s in %s()\n", parent_directory, __FUNCTION__);
    ppr_finalize_sdk();
    exit(EXIT_FAILURE);
  }

  item_list->filenames = (char**)malloc(item_list->size * sizeof(char*));
  sdk_utils_check_allocation(item_list->filenames, "item_list->filenames", __FUNCTION__);

  i = 0;
  while((entry = readdir(dir_stream)) != NULL)
  {
    if (entry->d_name[0] != '.' && entry->d_name[0] != '\0')
    {
      sprintf(filename, "%s/%s", parent_directory, entry->d_name);
      is_directory = sdk_utils_is_directory(filename);
      if ((item_type == SDK_UTILS_DIRECTORY_ITEM_ALL) ||
          (item_type == SDK_UTILS_DIRECTORY_ITEM_DIRECTORIES_ONLY && is_directory) ||
          (item_type == SDK_UTILS_DIRECTORY_ITEM_FILES_ONLY && !is_directory))
      {
        len = strlen(entry->d_name);
        item_list->filenames[i] = (char*)malloc((len + 1) * sizeof(char));
        sdk_utils_check_allocation(item_list->filenames[i], "item_list->filenames[i]", __FUNCTION__);
        strcpy(item_list->filenames[i], entry->d_name);
        i++;
      }
    }
  }

  closedir(dir_stream);

#endif
}

/******************************************************************************/
/*****************************************************************************/

/* Print the context configuration */
void sdk_utils_print_context_configuration(
  FILE *stream,
  ppr_context_type context)
{
  ppr_precision_type precision;
  ppr_coarse_detector_mask_type masks;
  ppr_landmark_detector_type landmarks;
  ppr_frontal_yaw_constraint_type yaw_constraint;
  int detection_threads;
  int aggressiveness;
  int min_size, max_size;
  float adaptive_min, adaptive_max;
  float detection_threshold;
  int x1, y1, x2, y2;
  ppr_enabled_status_type tracking_status;
  int tracking_cutoff;
  int num_visual_instances;
  ppr_enabled_status_type visual_attention_status;
  ppr_enabled_status_type shot_boundary_status;
  float shot_boundary_threshold;
  ppr_enabled_status_type recognition_status;
  int recognition_threads;
  ppr_template_extraction_type extraction_type;
  ppr_enabled_status_type template_thumbnails_status;

  TRY(ppr_get_detection_precision(context, &precision));

  if (precision == PPR_COARSE_PRECISION)
    TRY(ppr_get_coarse_detector_types(context, &masks));

  TRY(ppr_get_landmark_detector_type(context, &landmarks));
  TRY(ppr_get_frontal_yaw_constraint(context, &yaw_constraint));
  TRY(ppr_get_num_detection_threads(context, &detection_threads));
  TRY(ppr_get_search_pruning_aggressiveness(context, &aggressiveness));
  TRY(ppr_get_min_size(context, &min_size));
  TRY(ppr_get_max_size(context, &max_size));
  TRY(ppr_get_search_region(context, &x1, &y1, &x2, &y2));
  TRY(ppr_is_tracking_enabled(context, &tracking_status));

  if (tracking_status == PPR_ENABLED)
  {
    TRY(ppr_get_tracking_cutoff(context, &tracking_cutoff));
    TRY(ppr_get_num_visual_instances(context, &num_visual_instances));
    TRY(ppr_is_visual_attention_enabled(context, &visual_attention_status));
    TRY(ppr_is_shot_boundary_detection_enabled(context, &shot_boundary_status));

    if (shot_boundary_status == PPR_ENABLED)
      TRY(ppr_get_shot_boundary_threshold(context, &shot_boundary_threshold));
  }
  else
  {
    TRY(ppr_get_adaptive_min_size(context, &adaptive_min));
    TRY(ppr_get_adaptive_max_size(context, &adaptive_max));
    TRY(ppr_get_detection_threshold(context, &detection_threshold));
  }

  TRY(ppr_is_recognition_enabled(context, &recognition_status));

  if (recognition_status == PPR_ENABLED)
  {
    TRY(ppr_get_num_recognition_threads(context, &recognition_threads));
    TRY(ppr_get_template_extraction_type(context, &extraction_type));
    TRY(ppr_are_template_thumbnails_enabled(context, &template_thumbnails_status));
  }

  /* ----------------------------------------------------------------------- */

  fprintf(stream, "\n");
  fprintf(stream, "Context Configuration -------------------------------\n");
  fprintf(stream, "\n");
  fprintf(stream, "  detection precision           : %s\n", sdk_utils_get_precision_string(precision));
  if (precision == PPR_COARSE_PRECISION)
  {
    fprintf(stream, "  coarse detector masks         : ");
    if ((masks & PPR_FRONTAL_DETECTOR_MASK) == PPR_FRONTAL_DETECTOR_MASK)
      fprintf(stream, "frontal   ");
    if ((masks & PPR_RIGHT_PROFILE_DETECTOR_MASK) == PPR_RIGHT_PROFILE_DETECTOR_MASK)
      fprintf(stream, "right profile   ");
    if ((masks & PPR_LEFT_PROFILE_DETECTOR_MASK) == PPR_LEFT_PROFILE_DETECTOR_MASK)
      fprintf(stream, "left profile   ");
    if ((masks & PPR_FRONTAL_TILTED_N30_DETECTOR_MASK) == PPR_FRONTAL_TILTED_N30_DETECTOR_MASK)
      fprintf(stream, "frontal tilted -30   ");
    if ((masks & PPR_FRONTAL_TILTED_P30_DETECTOR_MASK) == PPR_FRONTAL_TILTED_P30_DETECTOR_MASK)
      fprintf(stream, "frontal tilted +30   ");
    fprintf(stream, "\n");
  }
  fprintf(stream, "  landmark detector             : %s\n", sdk_utils_get_landmark_detector_string(landmarks));
  fprintf(stream, "  frontal yaw constraint        : %s\n", sdk_utils_get_frontal_yaw_constraint_string(yaw_constraint));
  fprintf(stream, "  detection threads             : %d\n", detection_threads);
  fprintf(stream, "  search pruning aggressiveness : %d\n", aggressiveness);
  fprintf(stream, "  min size                      : %d\n", min_size);
  fprintf(stream, "  max size                      : %d\n", max_size);
  if (tracking_status == PPR_DISABLED)
  {
    fprintf(stream, "  adaptive min size             : %.2f\n", adaptive_min);
    fprintf(stream, "  adaptive max size             : %.2f\n", adaptive_max);
    fprintf(stream, "  detection threshold           : %.2f\n", detection_threshold);
  }
  fprintf(stream, "  search region                 : (%d, %d) x (%d, %d)\n", x1, y1, x2, y2);
  fprintf(stream, "\n");

  fprintf(stream, "  tracking                      : %s\n", sdk_utils_get_enabled_status_string(tracking_status));
  if (tracking_status == PPR_ENABLED)
  {
    fprintf(stream, "  tracking cutoff               : %d\n", tracking_cutoff);
    fprintf(stream, "  visual attention              : %s\n", sdk_utils_get_enabled_status_string(visual_attention_status));
    fprintf(stream, "  num visual instances          : %d\n", num_visual_instances);
    fprintf(stream, "  shot boundary detection       : %s\n", sdk_utils_get_enabled_status_string(shot_boundary_status));
    if (shot_boundary_status == PPR_ENABLED)
      fprintf(stream, "  shot boundary threshold       : %.2f\n", shot_boundary_threshold);
  }
  fprintf(stream, "\n");

  fprintf(stream, "  recognition                   : %s\n", sdk_utils_get_enabled_status_string(recognition_status));
  if (recognition_status == PPR_ENABLED)
  {
    fprintf(stream, "  recognition threads           : %d\n", recognition_threads);
    fprintf(stream, "  template extraction           : %s\n", sdk_utils_get_template_extraction_type_string(extraction_type));
    fprintf(stream, "  template thumbnails           : %s\n", sdk_utils_get_enabled_status_string(template_thumbnails_status));
  }
  fprintf(stream, "\n");

  fprintf(stream, "-----------------------------------------------------\n");
  fprintf(stream, "\n");
}


/*****************************************************************************
 * Detection & Tracking Constants
 *****************************************************************************/

#define SDK_UTILS_NUM_LANDMARK_CATEGORIES        8
#define SDK_UTILS_NUM_PRECISION_STRINGS          2
#define SDK_UTILS_NUM_LANDMARK_DETECTOR_STRINGS  5

/*****************************************************************************
 * Detection & Tracking Static Variables
 *****************************************************************************/

static const char *sdk_utils_static_landmark_category_strings[] =
{
  "LANDMARK_LEFT_EYE",
  "LANDMARK_RIGHT_EYE",
  "LANDMARK_NOSE_BASE",
  "LANDMARK_NOSE_BRIDGE",
  "LANDMARK_NOSE_TIP",
  "LANDMARK_NOSE_TOP",
  "LANDMARK_EYE_NOSE",
  "LANDMARK_MOUTH"
};

static const char *sdk_utils_static_precision_strings[] =
{
  "coarse",
  "fine"
};

static const char *sdk_utils_static_landmark_detector_strings[] =
{
  "none",
  "frontal",
  "multi-pose",
  "dual_frontal",
  "dual_multi-pose"
};

/*****************************************************************************
 * Detection & Tracking Print Functions
 *****************************************************************************/

/* Prints the contents of a ppr_polygon_type structure */
void sdk_utils_print_polygon(
  FILE *stream,
  ppr_polygon_type polygon,
  int indent)
{
  int i;

  char indent_str[SDK_UTILS_INDENT_STR_LEN];

  sdk_utils_get_indent_str(indent, indent_str);

  fprintf(stream, "%s - num_points: %d\n", indent_str, polygon.num_points);
  for (i = 0; i < polygon.num_points; i++)
    fprintf(stream, "%s  - point[%d] : (%8.3f, %8.3f)\n", indent_str, i, polygon.points[i].x, polygon.points[i].y);
}

/*****************************************************************************/

/* Prints the contents of a ppr_landmark_type structure */
void sdk_utils_print_landmark(
  FILE *stream,
  ppr_landmark_type landmark,
  int indent)
{

  char indent_str[SDK_UTILS_INDENT_STR_LEN];

  sdk_utils_get_indent_str(indent, indent_str);

  fprintf(stream, "%s  - category : %s\n",              indent_str, sdk_utils_get_landmark_category_string(landmark.category));
  fprintf(stream, "%s  - position : (%.3f, %.3f)\n",    indent_str, landmark.position.x, landmark.position.y);
}

/*****************************************************************************/

/* Prints a list of landmarks */
void sdk_utils_print_landmarks(
  FILE *stream,
  int num_landmarks,
  ppr_landmark_type *landmarks,
  int indent,
  int verbose)
{
  int i;

  char indent_str[SDK_UTILS_INDENT_STR_LEN];

  sdk_utils_get_indent_str(indent, indent_str);

  fprintf(stream, "%snum_landmarks     : %d\n", indent_str, num_landmarks);

  if (verbose)
  {
    for (i = 0; i < num_landmarks; i++)
    {
      fprintf(stream, "\n");
      sdk_utils_print_landmark(stream, landmarks[i], indent);
    }
  }
}

/*****************************************************************************/

/* Prints the contents of a ppr_object_type structure */
void sdk_utils_print_object(
  FILE *stream,
  ppr_object_type object,
  int indent,
  int verbose)
{
  ppr_error_type rv;
  
  ppr_polygon_type polygon;

  char indent_str[SDK_UTILS_INDENT_STR_LEN];

  sdk_utils_get_indent_str(indent, indent_str);

  rv = ppr_get_object_boundary(object, PPR_DEFAULT_BOUNDING_BOX, &polygon);
  if (rv != PPR_SUCCESS)
  {
    fprintf(stderr, "ERROR in sdk_utils_print_object():\n");
    fprintf(stderr, " - error returned from ppr_get_object_boundary(): %d-%s\n", rv, ppr_error_message(rv));
    return;
  }

  fprintf(stream, "%sposition          : (%.3f, %.3f)\n",    indent_str, object.position.x, object.position.y);
  fprintf(stream, "%sdimensions        : %.3f x %.3f (w x h)\n", indent_str, object.dimensions.width, object.dimensions.height);
  fprintf(stream, "%ssize              : %.3f\n",            indent_str, object.size);

  if (object.rotation.precision == PPR_COARSE_PRECISION)
    fprintf(stream, "%srotation (coarse) : %.3f, %.3f, %.3f (yaw, pitch, roll)\n", indent_str, object.rotation.yaw, object.rotation.pitch, object.rotation.roll);
  else
    fprintf(stream, "%srotation (fine)   : %.3f, %.3f, %.3f (yaw, pitch, roll)\n", indent_str, object.rotation.yaw, object.rotation.pitch, object.rotation.roll);

  fprintf(stream, "%sconfidence        : %.3f\n",            indent_str, object.confidence);
  
  sdk_utils_print_landmarks(stream, object.num_landmarks, object.landmarks, indent, verbose);

  if (verbose)
  {
    fprintf(stream, "\n");
    fprintf(stream, "%spolygon - using DEFAULT_BOUNDING_BOX\n", indent_str);
    sdk_utils_print_polygon(stream, polygon, indent);
  }

  ppr_free_polygon(polygon);
}

/*****************************************************************************/

/* Prints the contents of a ppr_object_list_type structure */
void sdk_utils_print_object_list(
  FILE *stream,
  ppr_object_list_type object_list,
  int indent,
  int verbose)
{
  int i;

  char indent_str[SDK_UTILS_INDENT_STR_LEN];

  sdk_utils_get_indent_str(indent, indent_str);

  if (object_list.num_objects >= 0)
  {
    fprintf(stream, "%s----------------------------------------------------------------------\n", indent_str);
    fprintf(stream, "%sNumber of Objects: %d\n", indent_str, object_list.num_objects);
    fprintf(stream, "%s----------------------------------------------------------------------\n", indent_str);

    for (i = 0; i < object_list.num_objects; i++)
    {
      fprintf(stream, "%s----------------------------------------------------------------------\n", indent_str);
      fprintf(stream, "%sObject[%d]\n", indent_str, i);
      fprintf(stream, "%s----------------------------------------------------------------------\n", indent_str);
      sdk_utils_print_object(stream, object_list.objects[i], indent, verbose);
      fprintf(stream, "\n");
    }

    fprintf(stream, "%s----------------------------------------------------------------------\n", indent_str);
  }
  else
  {
    fprintf(stream, "%s----------------------------------------------------------------------\n", indent_str);
    fprintf(stream, "%sINVALID OBJECT LIST\n", indent_str);
    fprintf(stream, "%s----------------------------------------------------------------------\n", indent_str);
  }
}

/*****************************************************************************/

/* Prints the contents of a ppr_tracked_object_type structure */
void sdk_utils_print_tracked_object(
  FILE *stream,
  ppr_tracked_object_type tracked_object,
  int indent,
  int verbose)
{
  char indent_str[SDK_UTILS_INDENT_STR_LEN];

  sdk_utils_get_indent_str(indent, indent_str);

  fprintf(stream, "%sindex             : %d\n",  indent_str, tracked_object.index);
  fprintf(stream, "%stime_stamp        : %lf\n", indent_str, tracked_object.time_stamp);
  fprintf(stream, "%stracked_id        : %d\n",  indent_str, tracked_object.tracked_id);
  fprintf(stream, "%strack_confidence  : %d\n",  indent_str, tracked_object.track_confidence);
 
  sdk_utils_print_object(stream, tracked_object.object, indent, verbose);
}

/*****************************************************************************/

/* Prints a list of tracked objects */
void sdk_utils_print_tracked_objects(
  FILE *stream,
  int num_tracked_objects,
  ppr_tracked_object_type *tracked_objects,
  int indent,
  int verbose)
{
  int i;

  char indent_str[SDK_UTILS_INDENT_STR_LEN];

  sdk_utils_get_indent_str(indent, indent_str);
  
  if (num_tracked_objects >= 0)
  {
    fprintf(stream, "%s----------------------------------------------------------------------\n", indent_str);
    fprintf(stream, "%sNumber of Tracked Objects: %d\n", indent_str, num_tracked_objects);
    fprintf(stream, "%s----------------------------------------------------------------------\n", indent_str);

    for (i = 0; i < num_tracked_objects; i++)
    {
      fprintf(stream, "%s----------------------------------------------------------------------\n", indent_str);
      fprintf(stream, "%sTracked_Object[%d]\n", indent_str, i);
      fprintf(stream, "%s----------------------------------------------------------------------\n", indent_str);
      sdk_utils_print_tracked_object(stream, tracked_objects[i], indent, verbose);
      fprintf(stream, "\n");
    }

    fprintf(stream, "%s----------------------------------------------------------------------\n", indent_str);
  }
  else
  {
    fprintf(stream, "%s----------------------------------------------------------------------\n", indent_str);
    fprintf(stream, "%sINVALID TRACKED OBJECT LIST\n", indent_str);
    fprintf(stream, "%s----------------------------------------------------------------------\n", indent_str);
  }
}

/*****************************************************************************/

/* Prints the contents of a ppr_tracked_object_list_type structure */
void sdk_utils_print_tracked_object_list(
  FILE *stream,
  ppr_tracked_object_list_type tracked_object_list,
  int indent,
  int verbose)
{
  /* Print list of tracked objects */
  sdk_utils_print_tracked_objects(stream,
                        tracked_object_list.num_tracked_objects,
                        tracked_object_list.tracked_objects,
                        indent,
                        verbose);
}

/*****************************************************************************/

/* Prints the contents of a ppr_track_summary_type structure */
void sdk_utils_print_track_summary(
  FILE *stream,
  ppr_track_summary_type track_summary,
  int indent)
{
  char indent_str[SDK_UTILS_INDENT_STR_LEN];

  sdk_utils_get_indent_str(indent, indent_str);

  fprintf(stream, "%stracked_id     : %d\n",                  indent_str, track_summary.tracked_id);
  fprintf(stream, "%sstart_index    : %d\n",                  indent_str, track_summary.start_index);
  fprintf(stream, "%sstop_index     : %d\n",                  indent_str, track_summary.stop_index);
  fprintf(stream, "%sstart_position : (%8.3f,%8.3f)\n",       indent_str, track_summary.start_position.x, track_summary.start_position.y);
  fprintf(stream, "%sstop_position  : (%8.3f,%8.3f)\n",       indent_str, track_summary.stop_position.x,  track_summary.stop_position.y);
  fprintf(stream, "%smin_dimensions : %.3f x %.3f (w x h)\n", indent_str, track_summary.min_dimensions.width, track_summary.min_dimensions.height);
  fprintf(stream, "%smax_dimensions : %.3f x %.3f (w x h)\n", indent_str, track_summary.max_dimensions.width, track_summary.max_dimensions.height);
  fprintf(stream, "%savg_dimensions : %.3f x %.3f (w x h)\n", indent_str, track_summary.avg_dimensions.width, track_summary.avg_dimensions.height);
  fprintf(stream, "%smin_confidence : %.3f\n",                indent_str, track_summary.min_confidence);
  fprintf(stream, "%smax_confidence : %.3f\n",                indent_str, track_summary.max_confidence);
  fprintf(stream, "%savg_confidence : %.3f\n",                indent_str, track_summary.avg_confidence);
}

/*****************************************************************************/

/* Prints the contents of a ppr_completed_track_type structure */
void sdk_utils_print_completed_track(
  FILE *stream,
  ppr_completed_track_type completed_track,
  int print_objects_in_track,
  int indent,
  int verbose)
{
  char indent_str[SDK_UTILS_INDENT_STR_LEN];

  sdk_utils_get_indent_str(indent, indent_str);

  sdk_utils_print_track_summary(stream, completed_track.track_summary, indent);
  if (print_objects_in_track)
  {
    fprintf(stream, "\n");
    sdk_utils_print_tracked_objects(stream, completed_track.num_tracked_objects, completed_track.tracked_objects, indent + 3, verbose);
  }
  else
  {
    fprintf(stream, "----------------------------------------------------------------------\n");
  }
}

/*****************************************************************************/

/* Prints the contents of a ppr_completed_track_list_type structure */
void sdk_utils_print_completed_track_list(
  FILE *stream,
  ppr_completed_track_list_type completed_track_list,
  int print_objects_in_track,
  int indent,
  int verbose)
{
  int i;

  char indent_str[SDK_UTILS_INDENT_STR_LEN];

  sdk_utils_get_indent_str(indent, indent_str);

  fprintf(stream, "%s----------------------------------------------------------------------\n", indent_str);
  fprintf(stream, "%sNumber of Completed Tracks: %d\n", indent_str, completed_track_list.num_completed_tracks);
  fprintf(stream, "%s----------------------------------------------------------------------\n", indent_str);
  
  for (i = 0; i < completed_track_list.num_completed_tracks; i++)
  {
    fprintf(stream, "%s----------------------------------------------------------------------\n", indent_str);
    fprintf(stream, "%sCompleted_Track[%d]\n", indent_str, i);
    fprintf(stream, "%s----------------------------------------------------------------------\n", indent_str);
    sdk_utils_print_completed_track(stream, completed_track_list.completed_tracks[i], print_objects_in_track, indent, verbose);
    fprintf(stream, "\n");
  }

  if (completed_track_list.num_completed_tracks == 0)
    fprintf(stream, "\n");
}

/*****************************************************************************
 * Detection & Tracking String Conversion Functions
 *****************************************************************************/

/* Returns a string for the ppr_landmark_category_type */
const char *sdk_utils_get_landmark_category_string(
  ppr_landmark_category_type category)
{
  if (category >= SDK_UTILS_NUM_LANDMARK_CATEGORIES)
    return "ILLEGAL_LANDMARK_CATEGORY";

  return sdk_utils_static_landmark_category_strings[category];
}

/*****************************************************************************/

/* Returns a string for the ppr_precision_type */
const char* sdk_utils_get_precision_string(
  ppr_precision_type precision)
{
  if (precision < 0 || precision >= SDK_UTILS_NUM_PRECISION_STRINGS)
    return "";

  return sdk_utils_static_precision_strings[(int)precision];
}

/*****************************************************************************/

/* Returns a string for the ppr_landmark_detector_type */
const char* sdk_utils_get_landmark_detector_string(
  ppr_landmark_detector_type landmark_detector)
{
  if (landmark_detector < 0 || landmark_detector >= SDK_UTILS_NUM_LANDMARK_DETECTOR_STRINGS)
    return "";

  return sdk_utils_static_landmark_detector_strings[(int)landmark_detector];
}

/*****************************************************************************/

/* Returns the ppr_precision_type specified by a string */
int sdk_utils_get_precision_from_string(
  const char *precision_str,
  ppr_precision_type *precision)
{
  if (strcmp(precision_str, "coarse") == 0)
    *precision = PPR_COARSE_PRECISION;
  else if (strcmp(precision_str, "fine") == 0)
    *precision = PPR_FINE_PRECISION;
  else
    return -1;

  return 0;
}

/*****************************************************************************/

/* Returns the ppr_landmark_detector_type specified by a string */
int sdk_utils_get_landmark_detector_from_string(
  const char *landmark_detector_str,
  ppr_landmark_detector_type *landmark_detector)
{
  if (strcmp(landmark_detector_str, "none") == 0)
    *landmark_detector = PPR_NO_LANDMARK_DETECTOR;
  else if (strcmp(landmark_detector_str, "frontal") == 0)
    *landmark_detector = PPR_FRONTAL_LANDMARK_DETECTOR;
  else if (strcmp(landmark_detector_str, "multi-pose") == 0)
    *landmark_detector = PPR_MULTI_POSE_LANDMARK_DETECTOR;
  else if (strcmp(landmark_detector_str, "dual_frontal") == 0)
    *landmark_detector = PPR_DUAL_FRONTAL_LANDMARK_DETECTOR;
  else if (strcmp(landmark_detector_str, "dual_multi-pose") == 0)
    *landmark_detector = PPR_DUAL_MULTI_POSE_LANDMARK_DETECTOR;
  else
    return -1;
  
  return 0;
}

/*****************************************************************************/

/* Returns the search region coordinates specified by a string */
int sdk_utils_get_search_region_from_string(
  const char *search_region_str,
  int *x1,
  int *y1,
  int *x2,
  int *y2)
{
  char *num;

  if (search_region_str == NULL)
    return -1;

  num = strtok((char*)search_region_str, ",");
  if (num == NULL)
    return -1;
  *x1 = atoi(num);

  num = strtok(NULL, ",");
  if (num == NULL)
    return -1;
  *y1 = atoi(num);

  num = strtok(NULL, ",");
  if (num == NULL)
    return -1;
  *x2 = atoi(num);

  num = strtok(NULL, ",");
  if (num == NULL)
    return -1;
  *y2 = atoi(num);

  return 0;
}



/*****************************************************************************
 * Miscellaneous Detection & Tracking Functions
 *****************************************************************************/

/* Write all visual instances of all completed tracks to specified directory */
void sdk_utils_write_visual_instances(
  const char *directory,
  const char *output_image_format,
  ppr_completed_track_list_type completed_track_list)
{
  int track_num;
  int instance_num;

  char instance_filename[FILENAME_MAX];

  for (track_num = 0; track_num < completed_track_list.num_completed_tracks; track_num++)
  {
    for (instance_num = 0; instance_num < completed_track_list.completed_tracks[track_num].num_visual_instances; instance_num++)
    {
      sprintf(instance_filename, "%s/id_%06d-instance_%02d_frame_%05d.%s",
              directory,
              completed_track_list.completed_tracks[track_num].track_summary.tracked_id,
              instance_num,
              completed_track_list.completed_tracks[track_num].visual_instances[instance_num].index,
              output_image_format);
      TRY_RAW_IMAGE(ppr_raw_image_io_write(instance_filename, completed_track_list.completed_tracks[track_num].visual_instances[instance_num].image));
    }
  }
}

/*****************************************************************************
 * Recognition Constants
 *****************************************************************************/

#define NUM_FRONTAL_YAW_CONSTRAINT_STRINGS    2
#define NUM_TEMPLATE_EXTRACTION_TYPE_STRINGS  2

/*****************************************************************************
 * Recognition Static Variables
 *****************************************************************************/

static const char *sdk_utils_static_frontal_yaw_constraint_strings[] =
{
  "restrictive",
  "permissive"
};

static const char *sdk_utils_static_template_extraction_type_strings[] =
{
  "single",
  "double"
};

/*****************************************************************************
 * Recognition Print Functions
 *****************************************************************************/

/* Prints the contents of a ppr_template_id_list_type structure */
void sdk_utils_print_template_id_list(
  FILE *stream,
  ppr_template_id_list_type template_id_list,
  int verbose)
{
  int i;

  fprintf(stream, "number of template ids: %d\n", template_id_list.num_template_ids);
  fprintf(stream, "capacity: %d\n", template_id_list.capacity);

  if (verbose && template_id_list.num_template_ids > 0)
  {
    fprintf(stream, "\n");
    for (i = 0; i < template_id_list.num_template_ids; i++)
      fprintf(stream, "%d ", template_id_list.template_ids[i]);
    fprintf(stream, "\n");
  }
}

/*****************************************************************************/

/* Prints the contents of a ppr_index_list_type structure */
void sdk_utils_print_index_list(
  FILE *stream,
  ppr_index_list_type index_list,
  int verbose)
{
  int i;

  fprintf(stream, "number of indices: %d\n", index_list.num_indices);
  fprintf(stream, "capacity: %d\n", index_list.capacity);

  if (verbose && index_list.num_indices > 0)
  {
    fprintf(stream, "\n");
    for (i = 0; i < index_list.num_indices; i++)
      fprintf(stream, "%d ", index_list.indices[i]);
    fprintf(stream, "\n");
  }
}

/*****************************************************************************/

/* Prints the contents of a ppr_score_list_type structure */
void sdk_utils_print_score_list(
  FILE *stream,
  ppr_score_list_type score_list,
  int verbose)
{
  int i;

  fprintf(stream, "number of scores: %d\n", score_list.num_scores);
  fprintf(stream, "capacity: %d\n", score_list.capacity);

  if (verbose && score_list.num_scores > 0)
  {
    fprintf(stream, "\n");
    for (i = 0; i < score_list.num_scores; i++)
      fprintf(stream, "%.3f ", score_list.scores[i]);
    fprintf(stream, "\n");
  }
}

/*****************************************************************************/

/* Prints the contents of a ppr_subject_list_type structure */
void sdk_utils_print_subject_list(
  FILE *stream,
  ppr_subject_list_type subject_list,
  int verbose)
{
  int i;
  int j;

  fprintf(stream, "number of subjects: %d\n", subject_list.num_subjects);
  fprintf(stream, "capacity: %d\n", subject_list.capacity);

  if (verbose && subject_list.num_subjects > 0)
  {
    fprintf(stream, "\n");
    for (i = 0; i < subject_list.num_subjects; i++)
    {
      fprintf(stream, "subject %3d: ", i);
      for (j = 0; j < subject_list.subjects[i].num_template_ids; j++)
        fprintf(stream, "%3d ", subject_list.subjects[i].template_ids[j]);
      fprintf(stream,"\n");
    }
  }
}

/*****************************************************************************/

/* Prints the contents of a ppr_similarity_matrix_type structure */
void sdk_utils_print_similarity_matrix(
  FILE *stream,
  ppr_context_type context,
  ppr_similarity_matrix_type similarity_matrix,
  int verbose)
{
  int i;
  int j;

  ppr_template_id_list_type query_row_template_id_list;
  ppr_template_id_list_type target_col_template_id_list;

  int num_query_rows;
  int num_target_cols;

  float score;

  TRY(ppr_get_query_template_id_list_from_similarity_matrix(context, similarity_matrix, &query_row_template_id_list));
  TRY(ppr_get_target_template_id_list_from_similarity_matrix(context, similarity_matrix, &target_col_template_id_list));

  num_query_rows = query_row_template_id_list.num_template_ids;
  num_target_cols = target_col_template_id_list.num_template_ids;

  fprintf(stream, "dimensions (query rows x target cols): %d x %d\n", num_query_rows, num_target_cols);

  if (verbose && num_query_rows > 0 && num_target_cols > 0)
  {
    fprintf(stream, "\n");
    fprintf(stream, " ID ");
    for (j = 0; j < num_target_cols; j++)
      fprintf(stream, "%7d ", target_col_template_id_list.template_ids[j]);
    fprintf(stream, "\n");
    
    for (i = 0; i < num_query_rows; i++)
    {
      fprintf(stream, "%3d ", query_row_template_id_list.template_ids[i]);
      for (j = 0; j < num_target_cols; j++)
      {
        TRY(ppr_get_similarity_matrix_element(context,
                                              similarity_matrix,
                                              query_row_template_id_list.template_ids[i],
                                              target_col_template_id_list.template_ids[j],
                                              &score));

        fprintf(stream, "%7.3f ", score);
      }
      fprintf(stream, "\n");
    }
  }

  ppr_free_template_id_list(query_row_template_id_list);
  ppr_free_template_id_list(target_col_template_id_list);
}

/*****************************************************************************/

/* Prints the contents of a ppr_gallery_type structure */
void sdk_utils_print_gallery(
  FILE *stream,
  ppr_context_type context,
  ppr_gallery_type gallery,
  int verbose)
{
  int i;
  int j;

  ppr_template_id_list_type template_id_list;
  ppr_template_relationship_type relationship;

  TRY(ppr_get_template_id_list_from_gallery(context, gallery, &template_id_list));

  fprintf(stream, "number of templates: %d\n", template_id_list.num_template_ids);
  fprintf(stream, "template ids: ");
  for (i = 0; i < template_id_list.num_template_ids; i++)
    fprintf(stream, "%d ", template_id_list.template_ids[i]);
  fprintf(stream, "\n");

  fprintf(stream, "\n");

  if (verbose)
  {
    fprintf(stream, "template relationships:\n");
    fprintf(stream, "\n");
    for (i = 0; i < template_id_list.num_template_ids; i++)
    {
      for (j = 0; j < template_id_list.num_template_ids; j++)
      {
        TRY(ppr_get_user_specified_template_relationship(context,
                                                         gallery,
                                                         template_id_list.template_ids[i],
                                                         template_id_list.template_ids[j],
                                                         &relationship));
        switch (relationship)
        {
        case PPR_RELATIONSHIP_UNKNOWN:
          fprintf(stream, ". ");
          break;
        case PPR_RELATIONSHIP_SAME_SUBJECT:
          fprintf(stream, "S ");
          break;
        case PPR_RELATIONSHIP_DIFFERENT_SUBJECTS:
          fprintf(stream, "D ");
          break;
        default:
          fprintf(stream, "\n");
          fprintf(stderr, "ERROR: invalid template relationship: %d\n", relationship);
          exit(EXIT_FAILURE);
        }
      }
      fprintf(stream, "\n");
    }
  }

  ppr_free_template_id_list(template_id_list);
}

/* Prints the contents of a ppr_inter_gallery_subject_list_type structure */
void sdk_utils_print_inter_gallery_subject_list(
  FILE *stream,
  ppr_inter_gallery_subject_list_type subject_list,
  int verbose)
{
  int i;
  int j;

  fprintf(stream, "number of subjects: %d\n", subject_list.num_subjects);
  fprintf(stream, "capacity: %d\n", subject_list.capacity);

  if (verbose && subject_list.num_subjects > 0)
  {
    fprintf(stream, "(gallery ID / template ID):\n");
    for (i = 0; i < subject_list.num_subjects; i++)    
    {
      fprintf(stream, "subject %3d: ", i);
      for (j = 0; j < subject_list.subjects[i].num_template_sources; j++)
      {
        fprintf(stream, "%d/", subject_list.subjects[i].template_sources[j].gallery_id);
        fprintf(stream, "%d", subject_list.subjects[i].template_sources[j].template_id);
        if (j != (subject_list.subjects[i].num_template_sources - 1))
          fprintf(stream, ", ");
      }
      fprintf(stream, "\n");
    }
  }
}

/*****************************************************************************
 * Recognition String Conversion Functions
 *****************************************************************************/

/* Returns a string for the ppr_frontal_yaw_constraint_type */
const char* sdk_utils_get_frontal_yaw_constraint_string(
  ppr_frontal_yaw_constraint_type frontal_yaw_constraint)
{
  if (frontal_yaw_constraint < 0 || frontal_yaw_constraint >= NUM_FRONTAL_YAW_CONSTRAINT_STRINGS)
    return "";

  return sdk_utils_static_frontal_yaw_constraint_strings[(int)frontal_yaw_constraint];
}

/*****************************************************************************/

/* Returns the ppr_frontal_yaw_constraint_type specified by a string */
int sdk_utils_get_frontal_yaw_constraint_from_string(
  const char *frontal_yaw_constraint_str,
  ppr_frontal_yaw_constraint_type *frontal_yaw_constraint)
{
  if (strcmp(frontal_yaw_constraint_str, "restrictive") == 0)
    *frontal_yaw_constraint = PPR_FRONTAL_YAW_CONSTRAINT_RESTRICTIVE;
  else if (strcmp(frontal_yaw_constraint_str, "permissive") == 0)
    *frontal_yaw_constraint = PPR_FRONTAL_YAW_CONSTRAINT_PERMISSIVE;
  else
    return -1;
  
  return 0;
}

/*****************************************************************************/

/* Returns a string for the ppr_template_extraction_type */
const char* sdk_utils_get_template_extraction_type_string(
  ppr_template_extraction_type extraction_type)
{
  if (extraction_type < 0 || extraction_type >= NUM_TEMPLATE_EXTRACTION_TYPE_STRINGS)
    return "";

  return sdk_utils_static_template_extraction_type_strings[(int)extraction_type];
}

/*****************************************************************************/

/* Returns the ppr_template_extraction_type specified by a string */
int sdk_utils_get_template_extraction_type_from_string(
  const char *extraction_type_str,
  ppr_template_extraction_type *extraction_type)
{
  if (strcmp(extraction_type_str, "single") == 0)
    *extraction_type = PPR_EXTRACT_SINGLE;
  else if (strcmp(extraction_type_str, "double") == 0)
    *extraction_type = PPR_EXTRACT_DOUBLE;
  else
    return -1;

  return 0;
}

/**********************************************************************
 * Miscellaneous Recognition Functions
 **********************************************************************/

/* Writes template thumbnail to the specified file.  Writes gray image if template has no thumbnail */
void sdk_utils_write_template_thumbnail(
  const char *file_name,
  ppr_context_type context,
  ppr_template_type the_template)
{
  int i;
  
  ppr_error_type err;
  ppr_raw_image_type thumbnail;
  
  err = ppr_get_template_thumbnail(context, the_template, &thumbnail);
  if (err != PPR_SUCCESS)
  {
    if (err == PPR_TEMPLATE_DOES_NOT_HAVE_THUMBNAIL)
    {
      TRY_RAW_IMAGE(ppr_raw_image_create(&thumbnail, 48, 64, PPR_RAW_IMAGE_GRAY8));
      for (i = 0; i < 48 * 64; i++)
        thumbnail.data[i] = 128;
    }
    else
    {
      sdk_utils_check_ppr_error(err, "sdk_utils_write_template_thumbnail");
    }
  }
  
  TRY_RAW_IMAGE(ppr_raw_image_io_write(file_name, thumbnail));
  ppr_raw_image_free(thumbnail);
}

/******************************************************************************/

/* Writes thumbnail of template ID to the specified file.  Writes gray image if template has no thumbnail */
void sdk_utils_write_template_thumbnail_by_id(
  const char *file_name,
  ppr_context_type context,
  ppr_gallery_type gallery,
  int template_id)
{
  int i;
  
  ppr_error_type err;
  ppr_raw_image_type thumbnail;
  
  err = ppr_get_template_thumbnail_by_id(context, gallery, template_id, &thumbnail);
  if (err != PPR_SUCCESS)
  {
    if (err == PPR_TEMPLATE_DOES_NOT_HAVE_THUMBNAIL)
    {
      TRY_RAW_IMAGE(ppr_raw_image_create(&thumbnail, 48, 64, PPR_RAW_IMAGE_GRAY8));
      for (i = 0; i < 48 * 64; i++)
        thumbnail.data[i] = 128;
    }
    else
    {
      sdk_utils_check_ppr_error(err, "sdk_utils_write_template_thumbnail_by_id");
    }
  }
  
  TRY_RAW_IMAGE(ppr_raw_image_io_write(file_name, thumbnail));
  ppr_raw_image_free(thumbnail);
}

/******************************************************************************/

/* Writes thumbnails of all templates in a template id list to a directory */
void sdk_utils_write_template_id_list_thumbnails(
  const char *directory,
  ppr_context_type context,
  ppr_gallery_type gallery,
  int write_source_images,
  ppr_template_id_list_type template_id_list)
{
  int i;

  FILE *fid = NULL;

  ppr_template_type tmp_template;
  char thumbnail_filename[FILENAME_MAX];
  char text_filename[FILENAME_MAX];
  char template_string[PPR_MAX_TEMPLATE_STRING_SIZE];
  char source_string[PPR_MAX_TEMPLATE_STRING_SIZE];

  sdk_utils_make_directory(directory);

  if (write_source_images)
  {
    sprintf(text_filename, "%s/source_images.txt", directory);
    fid = fopen(text_filename, "wt");
    sdk_utils_check_file_open(fid, text_filename, __FUNCTION__);
  }

  for (i = 0; i < template_id_list.num_template_ids; i++)
  {
    TRY(ppr_get_template_by_id(context, gallery, template_id_list.template_ids[i], &tmp_template));
    sprintf(thumbnail_filename, "%s/thumbnail_%02d.jpg", directory, i);
    sdk_utils_write_template_thumbnail(thumbnail_filename, context, tmp_template);
    ppr_free_template(tmp_template);

    if (write_source_images)
    {
      TRY(ppr_get_template_string_by_id(context, gallery, template_id_list.template_ids[i], template_string));
      sdk_utils_get_item_from_template_string(template_string, "#SOURCE", source_string);
      fprintf(fid, "thumbnail %02d source image: %s\n", i, source_string);
    }
  }

  if (write_source_images)
    fclose(fid);
}

/******************************************************************************/

/* Writes a directory structure of thumbnails where each directory is a single subject */
void sdk_utils_write_subject_list_thumbnails(
  const char *directory,
  ppr_context_type context,
  ppr_gallery_type gallery,
  int write_source_images,
  ppr_subject_list_type subject_list)
{
  int i;
  char curr_subject_directory[FILENAME_MAX];

  for (i = 0; i < subject_list.num_subjects; i++)
  {
    sprintf(curr_subject_directory, "%s/subject_%02d", directory, i);
    sdk_utils_write_template_id_list_thumbnails(curr_subject_directory, context, gallery, write_source_images,
                                                subject_list.subjects[i]);
  }
}

/******************************************************************************/

/* Adds an item to a template string, marking the item with the specifed marker string */
void sdk_utils_add_item_to_template_string(
  char *template_string,
  const char *marker_string,
  char *item_string)
{
  char new_string[PPR_MAX_TEMPLATE_STRING_SIZE];

  if (strlen(template_string) + strlen(marker_string) + strlen(item_string) + 2 >= PPR_MAX_TEMPLATE_STRING_SIZE)
  {
    fprintf(stderr, "New template string is too long in %s\n", __FUNCTION__);
    exit(EXIT_FAILURE);
  }
  
  if (template_string[0] == '\0')
  {
    sprintf(template_string, "%s=%s", marker_string, item_string);
  }
  else
  {
    sprintf(new_string, "%s;%s=%s", template_string, marker_string, item_string);
    strcpy(template_string, new_string);
  }
}

/******************************************************************************/

/* Gets an item from a template string where the item is marked by the specifed marker string */
void sdk_utils_get_item_from_template_string(
  const char *template_string,
  const char *marker_string,
  char *item_string)
{
  int i;
  char *substring;
  
  /* Get pointer to beginning of marker */
  substring = strstr(template_string, marker_string);

  /* Return empty string if marker is not present */
  if (substring == NULL)
  {
    item_string[0] = '\0';
    return;
  }

  /* Advance past marker */
  substring += strlen(marker_string);
  if (substring[0] != '=')
  {
    item_string[0] = '\0';
    return;
  }
  substring++;

  /* Copy until ; or end is reached */
  for (i = 0; substring[i] != ';' && substring[i] != '\0'; i++)
    item_string[i] = substring[i];

  /* Put null terminator at end of string */
  item_string[i] = '\0';
}
