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
 * File: sdk_draw_utils.h
 *
 * This file provides utility functions that are not part of the PittPatt SDK,
 * but can be useful when writing programs that use the SDK.
 *
 * These functions provide an easy way to draw on images.
 */

#ifndef __SDK_DRAW_UTILS_H
#define __SDK_DRAW_UTILS_H

#include "pittpatt_detection.h"

/* If C++ then we need to __extern "C". Compiler defines __cplusplus */
#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Constants
 ******************************************************************************/

/* Skipping last column because it is unnecessary padding */
#define SDK_DRAW_UTILS_CHAR_WIDTH  7

/* Added blank top row so background color surrounds text by 1 pixels on all sides */
#define SDK_DRAW_UTILS_CHAR_HEIGHT 9

/*******************************************************************************
 * Enumerated Types
 ******************************************************************************/

typedef enum
{
  SDK_DRAW_UTILS_GREEN,
  SDK_DRAW_UTILS_RED,
  SDK_DRAW_UTILS_BLUE,
  SDK_DRAW_UTILS_YELLOW,
  SDK_DRAW_UTILS_CYAN,
  SDK_DRAW_UTILS_MAGENTA,
  SDK_DRAW_UTILS_ORANGE,
  SDK_DRAW_UTILS_PINK,
  SDK_DRAW_UTILS_BROWN,
  SDK_DRAW_UTILS_TAN,
  SDK_DRAW_UTILS_GRAY,
  SDK_DRAW_UTILS_BLACK,
  SDK_DRAW_UTILS_WHITE,
  SDK_DRAW_UTILS_NUM_COLORS,
  SDK_DRAW_UTILS_TRANSPARENT
} sdk_draw_utils_color_type;

typedef enum
{
  SDK_DRAW_UTILS_SOLID_LINE,
  SDK_DRAW_UTILS_DOTTED_LINE
} sdk_draw_utils_line_type;

/*******************************************************************************
 * Structure Types
 ******************************************************************************/

typedef struct
{
  int x;
  int y;
} sdk_draw_utils_point_type;
  
typedef struct
{
  sdk_draw_utils_color_type color;
  sdk_draw_utils_line_type line_type;
  int thickness;
} sdk_draw_utils_line_attributes_type;

typedef struct
{
  sdk_draw_utils_color_type color;
  sdk_draw_utils_color_type background_color;
  int scale;
} sdk_draw_utils_text_attributes_type;  

/*****************************************************************************
 * Functions
 *****************************************************************************/

/* Initializes line attributes structure with reasonable defaults */
void sdk_draw_utils_init_line_attributes(
  sdk_draw_utils_line_attributes_type *attributes);

/* Initializes text attributes structure with reasonable defaults */
void sdk_draw_utils_init_text_attributes(
  sdk_draw_utils_text_attributes_type *attributes);

/* Draws a single pixel on an image in the image's color space */
void sdk_draw_utils_draw_pixel(
  ppr_raw_image_type *raw_image,
  sdk_draw_utils_point_type point,
  sdk_draw_utils_color_type color);

/* Draws a line on an image */
void sdk_draw_utils_draw_line(
  ppr_raw_image_type *raw_image,
  ppr_coordinate_type point1,
  ppr_coordinate_type point2,
  sdk_draw_utils_line_attributes_type attributes);

/* Draws a cross (plus sign) on an image */
void sdk_draw_utils_draw_cross(
  ppr_raw_image_type *raw_image,
  ppr_coordinate_type center,
  int size,
  sdk_draw_utils_line_attributes_type attributes);

/* Draws a polygon on an image */
void sdk_draw_utils_draw_polygon(
  ppr_raw_image_type *raw_image,
  ppr_polygon_type polygon,
  sdk_draw_utils_line_attributes_type attributes);

/* Draws a single character on an image at the specified position */
void sdk_draw_utils_draw_char(
  ppr_raw_image_type *raw_image,
  char c,
  sdk_draw_utils_point_type top_left,
  sdk_draw_utils_text_attributes_type attributes);

/* Draws a string on an image at the specified position */
void sdk_draw_utils_draw_string(
  ppr_raw_image_type *image,
  const char *c,
  sdk_draw_utils_point_type top_left,
  sdk_draw_utils_text_attributes_type attributes);

/* Returns a string for the sdk_draw_utils_color_type */
const char *sdk_draw_utils_get_color_string(
  sdk_draw_utils_color_type color);

/*****************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* __SDK_DRAW_UTILS_H */
