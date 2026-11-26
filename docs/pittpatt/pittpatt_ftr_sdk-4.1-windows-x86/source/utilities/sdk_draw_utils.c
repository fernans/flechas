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
 * File: sdk_draw_utils.c
 *
 * This file provides utility functions that are not part of the PittPatt SDK,
 * but can be useful when writing programs that use the SDK.
 *
 * These functions provide an easy way to draw on images.
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "sdk_draw_utils.h"
#include "sdk_draw_utils_charset.h"

/******************************************************************************
 * Constants
 ******************************************************************************/

#define SDK_UTILS_DOTTED_LINE_ON_LENGTH  10
#define SDK_UTILS_DOTTED_LINE_OFF_LENGTH  3

static const char *sdk_draw_utils_static_color_strings[] =
{
  "green",
  "red",
  "blue",
  "yellow",
  "cyan",
  "magenta",
  "orange",
  "pink",
  "brown",
  "tan",
  "gray",
  "black",
  "white"
};

/*******************************************************************************
 * Macros
 ******************************************************************************/

/* Clips a value at 0 if it is less than 0 and at 255 if it is greater than 255 */
#define CLIP8(AA) (unsigned char)(((int)(AA)<0)?0:((int)(AA)>0xFF)?0xFF:(int)(AA))

/******************************************************************************
 * Local Function Prototypes
 ******************************************************************************/

static int sdk_draw_utils_bit_at(
  unsigned char c,
  int x,
  int y);

static void sdk_draw_utils_color_to_rgb(
  sdk_draw_utils_color_type color,
  unsigned char *r,
  unsigned char *g,
  unsigned char *b);

static void sdk_draw_utils_rgb24_to_gray8(
  unsigned char r,
  unsigned char g,
  unsigned char b,
  unsigned char *gray);

static void sdk_draw_utils_rgb24_to_argb32(
  unsigned char r,
  unsigned char g,
  unsigned char b,
  unsigned int *argb);

static void sdk_draw_utils_rgb24_to_yuv24(
  unsigned char r,
  unsigned char g,
  unsigned char b,
  unsigned char *y,
  unsigned char *u,
  unsigned char *v);

/******************************************************************************
 * Public Functions
 ******************************************************************************/

/* Initializes line attributes structure with reasonable defaults */
void sdk_draw_utils_init_line_attributes(
  sdk_draw_utils_line_attributes_type *attributes)
{
  attributes->color = SDK_DRAW_UTILS_GREEN;
  attributes->line_type = SDK_DRAW_UTILS_SOLID_LINE;
  attributes->thickness = 1;
}

/******************************************************************************/

/* Initializes text attributes structure with reasonable defaults */
void sdk_draw_utils_init_text_attributes(
  sdk_draw_utils_text_attributes_type *attributes)
{
  attributes->color = SDK_DRAW_UTILS_GREEN;
  attributes->background_color = SDK_DRAW_UTILS_TRANSPARENT;
  attributes->scale = 1;
}

/******************************************************************************/

/* Draws a single pixel on an image in the image's color space */
void sdk_draw_utils_draw_pixel(
  ppr_raw_image_type *raw_image,
  sdk_draw_utils_point_type point,
  sdk_draw_utils_color_type color)
{
  int x;
  int y;
  
  unsigned char r;
  unsigned char g;
  unsigned char b;

  unsigned char gray;
  
  unsigned char y_chan;
  unsigned char u_chan;
  unsigned char v_chan;

  unsigned int argb;
  unsigned int *p_argb;

  /* Don't draw pixel if color is set to transparent */
  if (color == SDK_DRAW_UTILS_TRANSPARENT)
    return;

  x = point.x;
  y = point.y;
  
  /* Don't try to draw pixel if it is out of bounds */
  if (x < 0 || x >= raw_image->width || y < 0 || y >= raw_image->height)
    return;
  
  sdk_draw_utils_color_to_rgb(color, &r, &g, &b);
  
  switch (raw_image->color_space)
  {
    case PPR_RAW_IMAGE_GRAY8:
      sdk_draw_utils_rgb24_to_gray8(r, g, b, &gray);
      raw_image->data[y * raw_image->bytes_per_line + x] = gray;
      break;
      
    case PPR_RAW_IMAGE_RGB24:
      raw_image->data[y * raw_image->bytes_per_line + x * 3 + 0] = r;
      raw_image->data[y * raw_image->bytes_per_line + x * 3 + 1] = g;
      raw_image->data[y * raw_image->bytes_per_line + x * 3 + 2] = b;
      break;
      
    case PPR_RAW_IMAGE_BGR24:
      raw_image->data[y * raw_image->bytes_per_line + x * 3 + 0] = b;
      raw_image->data[y * raw_image->bytes_per_line + x * 3 + 1] = g;
      raw_image->data[y * raw_image->bytes_per_line + x * 3 + 2] = r;
      break;
      
    case PPR_RAW_IMAGE_ARGB32:
      sdk_draw_utils_rgb24_to_argb32(r, g, b, &argb);
      p_argb = (unsigned int*)&raw_image->data[y * raw_image->bytes_per_line + x * 4];
      *p_argb = argb;
      break;
      
    case PPR_RAW_IMAGE_YUV:
      sdk_draw_utils_rgb24_to_yuv24(r, g, b, &y_chan, &u_chan, &v_chan);
      raw_image->data[y * raw_image->bytes_per_line + x * 3 + 0] = y_chan;
      raw_image->data[y * raw_image->bytes_per_line + x * 3 + 1] = u_chan;
      raw_image->data[y * raw_image->bytes_per_line + x * 3 + 2] = v_chan;
      break;

    default:
      fprintf(stderr, "ERROR: illegal color space in sdk_draw_utils_draw_pixel()\n");
      exit(EXIT_FAILURE);
  }
}

/******************************************************************************/

/* Draws a line on an image */
void sdk_draw_utils_draw_line(
  ppr_raw_image_type *raw_image,
  ppr_coordinate_type point1,
  ppr_coordinate_type point2,
  sdk_draw_utils_line_attributes_type attributes)
{
  int upper_offset;
  int lower_offset;
  ppr_coordinate_type diff;
  float slope;
  float intercept;
  float inverse_slope;
  int start;
  int stop;
  int x;
  int y;
  int i;
  int j;
  int width;
  int height;
  int draw_count;
  sdk_draw_utils_point_type pixel;

  if (attributes.thickness <= 0)
  {
    fprintf(stderr, "ERROR: draw_line() requires thickness > 0\n");
    return;
  }

  width = raw_image->width;
  height = raw_image->height;

  /* Do not draw line if both points are outside of the image */
  if ((point1.x < 0 || point1.x >= width || point1.y < 0 || point1.y >= height) &&
      (point2.x < 0 || point2.x >= width || point2.y < 0 || point2.y >= height))
  {
    return;
  }

  if ((attributes.thickness % 2) == 0)
  {
    lower_offset = attributes.thickness / 2;
    upper_offset = (attributes.thickness - 1) / 2;
  }
  else
  {
    lower_offset = attributes.thickness / 2;
    upper_offset = attributes.thickness / 2;
  }
  
  diff.x = point2.x - point1.x;
  diff.y = point2.y - point1.y;
  
  if (fabs(diff.y) < fabs(diff.x))
  {
    slope = diff.y / diff.x;
    intercept = point1.y - slope * point1.x;

    if (point1.x > point2.x)
    {
      start = (int)point2.x;
      stop  = (int)point1.x;
    }
    else
    {
      start = (int)point1.x;
      stop  = (int)point2.x;
    }

    if (start < 0)
      start = 0;
    if (stop >= width)
      stop = width - 1;

    x = start;
    draw_count = 0;
    while (x <= stop)
    {
      y = (int)(slope * (float)x + intercept);

      for (i = y - lower_offset; i <= y + upper_offset; i++)
      {
        pixel.x = x;
        pixel.y = i;
        sdk_draw_utils_draw_pixel(raw_image, pixel, attributes.color);
      }

      x++;

      if (attributes.line_type == SDK_DRAW_UTILS_DOTTED_LINE)
      {
        draw_count++;
        if (draw_count == SDK_UTILS_DOTTED_LINE_ON_LENGTH)
        {
          draw_count = 0;
          x += SDK_UTILS_DOTTED_LINE_OFF_LENGTH;
        }
      }
    }
  }
  else
  {
    inverse_slope = diff.x / diff.y;
    intercept = point1.x - point1.y * inverse_slope;

    if (point1.y > point2.y)
    {
      start = (int)point2.y;
      stop  = (int)point1.y;
    }
    else
    {
      start = (int)point1.y;
      stop  = (int)point2.y;
    }

    if (start < 0)
      start = 0;
    if (stop >= height)
      stop = height - 1;

    y = start;
    draw_count = 0;
    while (y <= stop)
    {
      x = (int)((float)y * inverse_slope + intercept);

      for (j = x - lower_offset; j <= x + upper_offset; j++)
      {
        pixel.x = j;
        pixel.y = y;
        sdk_draw_utils_draw_pixel(raw_image, pixel, attributes.color);
      }

      y++;

      if (attributes.line_type == SDK_DRAW_UTILS_DOTTED_LINE)
      {
        draw_count++;
        if (draw_count == SDK_UTILS_DOTTED_LINE_ON_LENGTH)
        {
          draw_count = 0;
          y += SDK_UTILS_DOTTED_LINE_OFF_LENGTH;
        }
      }
    }
  }
}

/*****************************************************************************/

/* Draw a cross (plus sign) on an image */
void sdk_draw_utils_draw_cross(
  ppr_raw_image_type *raw_image,
  ppr_coordinate_type center,
  int size,
  sdk_draw_utils_line_attributes_type attributes)
{
  ppr_coordinate_type line1_pnt1;
  ppr_coordinate_type line1_pnt2;
  ppr_coordinate_type line2_pnt1;
  ppr_coordinate_type line2_pnt2;

  line1_pnt1.x = center.x - size / 2;
  line1_pnt1.y = center.y;

  line1_pnt2.x = center.x + size / 2;
  line1_pnt2.y = center.y;

  sdk_draw_utils_draw_line(raw_image, line1_pnt1, line1_pnt2, attributes);

  line2_pnt1.x = center.x;
  line2_pnt1.y = center.y - size / 2;

  line2_pnt2.x = center.x;
  line2_pnt2.y = center.y + size / 2;

  sdk_draw_utils_draw_line(raw_image, line2_pnt1, line2_pnt2, attributes);
}

/******************************************************************************/

/* Draws a polygon on an image */
void sdk_draw_utils_draw_polygon(
  ppr_raw_image_type *raw_image,
  ppr_polygon_type polygon,
  sdk_draw_utils_line_attributes_type attributes)
{
  int i;
  
  for (i = 0; i < polygon.num_points; i++)
    sdk_draw_utils_draw_line(raw_image, polygon.points[i], polygon.points[(i + 1) % polygon.num_points], attributes);
}

/******************************************************************************/

/* Draws a single character on an image at the specified position */
void sdk_draw_utils_draw_char(
  ppr_raw_image_type *raw_image,
  char c,
  sdk_draw_utils_point_type top_left,
  sdk_draw_utils_text_attributes_type attributes)
{
  int y;
  int x;
  int i;
  int j;
  int img_x;
  int img_y;
  sdk_draw_utils_point_type pixel;

  if (top_left.x >= raw_image->width || top_left.y >= raw_image->height)
    return;

  img_y = top_left.y;
  for (y = 0; y < SDK_DRAW_UTILS_CHAR_HEIGHT; y++)
  {
    for (i = 0; i < attributes.scale && img_y < raw_image->height; i++)
    {
      img_x = top_left.x;

      for (x = 0; x < SDK_DRAW_UTILS_CHAR_WIDTH; x++)
      {
        for (j = 0; j < attributes.scale && img_x < raw_image->width; j++)
        {
          pixel.x = img_x;
          pixel.y = img_y;
          
          if (sdk_draw_utils_bit_at(c, x, y))
            sdk_draw_utils_draw_pixel(raw_image, pixel, attributes.color);
          else
            sdk_draw_utils_draw_pixel(raw_image, pixel, attributes.background_color);
          
          img_x++;
        }
      }
      img_y++;
      
      if (img_y >= raw_image->height)
        return;
    }
  }
}

/******************************************************************************/

/* Draws a string on an image at the specified position */
void sdk_draw_utils_draw_string(
  ppr_raw_image_type *raw_image,
  const char *c,
  sdk_draw_utils_point_type top_left,
  sdk_draw_utils_text_attributes_type attributes)
{
  int letter;
  int original_x = top_left.x;
  
  letter = *c;
  while (letter != '\0')
  {
    if (letter == '\n')
    {
      top_left.x = original_x;
      top_left.y += SDK_DRAW_UTILS_CHAR_HEIGHT * attributes.scale;
    }
    else
    {
      sdk_draw_utils_draw_char(raw_image, (char)letter, top_left, attributes);
      top_left.x += SDK_DRAW_UTILS_CHAR_WIDTH * attributes.scale;
    }
    
    c++;
    letter = *c;
  }
}

/******************************************************************************/

/* Returns a string for the sdk_utils_color_type */
const char *sdk_draw_utils_get_color_string(
  sdk_draw_utils_color_type color)
{
  if (color >= SDK_DRAW_UTILS_NUM_COLORS || color < 0)
    return "ILLEGAL_COLOR";

  return sdk_draw_utils_static_color_strings[color];
}

/******************************************************************************
 * Local Functions
 ******************************************************************************/

static int sdk_draw_utils_bit_at(
  unsigned char c,
  int x,
  int y)
{
  unsigned char row;
  
  if (c < 0x20)
    c = 0x20;
  
  c -= 0x20;

  /*
   * Characters are 5 x 7 right-justified in an 8x9 bit field.
   * Shifting to the left to center them.  Since the width is odd, it can't be
   * perfectly centered.  Choosing to err to the left side and draw_char ignores
   * the last column.
   */
  row = charset[c][y] << 2;
  
  return (row >> (7-x)) & 1;
}

/******************************************************************************/

static void sdk_draw_utils_color_to_rgb(
  sdk_draw_utils_color_type color,
  unsigned char *r,
  unsigned char *g,
  unsigned char *b)
{
  switch (color)
  {
  case SDK_DRAW_UTILS_BLACK:
    *r = 0x00; *g = 0x00; *b = 0x00;
    break;
  case SDK_DRAW_UTILS_WHITE:
    *r = 0xff; *g = 0xff; *b = 0xff;
    break;
  case SDK_DRAW_UTILS_GRAY:
    *r = 0x80; *g = 0x80; *b = 0x80;
    break;
  case SDK_DRAW_UTILS_TAN:
    *r = 0xd2; *g = 0xb4; *b = 0x8c;
    break;
  case SDK_DRAW_UTILS_BROWN:
    *r = 0xa5; *g = 0x2a; *b = 0x2a;
    break;
  case SDK_DRAW_UTILS_PINK:
    *r = 0xff; *g = 0xc0; *b = 0xcb;
    break;
  case SDK_DRAW_UTILS_RED:
    *r = 0xff; *g = 0x00; *b = 0x00;
    break;
  case SDK_DRAW_UTILS_ORANGE:
    *r = 0xff; *g = 0x80; *b = 0x00;
    break;
  case SDK_DRAW_UTILS_YELLOW:
    *r = 0xff; *g = 0xff; *b = 0x00;
    break;
  case SDK_DRAW_UTILS_GREEN:
    *r = 0x00; *g = 0xff; *b = 0x00;
    break;
  case SDK_DRAW_UTILS_CYAN:
    *r = 0x00; *g = 0xff; *b = 0xff;
    break;
  case SDK_DRAW_UTILS_BLUE:
    *r = 0x00; *g = 0x00; *b = 0xff;
    break;
  case SDK_DRAW_UTILS_MAGENTA:
    *r = 0xff; *g = 0x00; *b = 0xff;
    break;
  default:
    fprintf(stderr, "ERROR: illegal color in sdk_draw_utils_color_to_rgb()\n");
    fprintf(stderr, "  Defaulting to BLACK\n");
    *r = 0x00; *g = 0x00; *b = 0x00;
    break;
  }
}

/******************************************************************************/

static void sdk_draw_utils_rgb24_to_gray8(
  unsigned char r,
  unsigned char g,
  unsigned char b,
  unsigned char *gray)
{
  float grayf;
  
  grayf = (0.299f * (float)r + 0.587f * (float)g + 0.114f * (float)b);
  *gray = CLIP8(grayf);
}

/******************************************************************************/

static void sdk_draw_utils_rgb24_to_argb32(
  unsigned char r,
  unsigned char g,
  unsigned char b,
  unsigned int *argb)
{
  *argb = (0xFF << 24) | (r << 16) | (g << 8) | (b);
}

/******************************************************************************/

static void sdk_draw_utils_rgb24_to_yuv24(
  unsigned char r,
  unsigned char g,
  unsigned char b,
  unsigned char *y,
  unsigned char *u,
  unsigned char *v)
{
  float yf;
  float uf;
  float vf;

  yf = ( 0.2990f * (float)r) + ( 0.5870f * (float)g) + ( 0.1140f * (float)b);
  uf = (-0.1687f * (float)r) + (-0.3313f * (float)g) + ( 0.5000f * (float)b) + 128.0f;
  vf = ( 0.5000f * (float)r) + (-0.4187f * (float)g) + (-0.0813f * (float)b) + 128.0f;

  *y = CLIP8(yf);
  *u = CLIP8(uf);
  *v = CLIP8(vf);
}

/******************************************************************************/
