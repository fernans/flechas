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


#ifndef __PITTPATT_RAW_IMAGE_H
#define __PITTPATT_RAW_IMAGE_H

/* If C++ then we need to extern "C". Compiler defines __cplusplus */
#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Enumerated Types
 ******************************************************************************/

typedef enum
{
  PPR_RAW_IMAGE_GRAY8,
  PPR_RAW_IMAGE_RGB24,
  PPR_RAW_IMAGE_BGR24,
  PPR_RAW_IMAGE_ARGB32,
  PPR_RAW_IMAGE_YUV     /* uses full range [0-255] (as in JPEG standard) */
} ppr_raw_image_color_space_type;

typedef enum
{
  PPR_RAW_IMAGE_SUCCESS,
  PPR_RAW_IMAGE_FAILURE,
  PPR_RAW_IMAGE_INVALID_COLOR_SPACE,
  PPR_RAW_IMAGE_UNABLE_TO_READ_FILE,
  PPR_RAW_IMAGE_UNABLE_TO_WRITE_FILE
} ppr_raw_image_error_type;

/*******************************************************************************
 * Structure Types
 ******************************************************************************/

typedef struct
{
  unsigned char *data;
  int width;
  int height;
  int bytes_per_line;
  ppr_raw_image_color_space_type color_space;
} ppr_raw_image_type;

/*******************************************************************************
 * Functions
 ******************************************************************************/

ppr_raw_image_error_type ppr_raw_image_create (ppr_raw_image_type *image,
                                               int width,
                                               int height,
                                               ppr_raw_image_color_space_type color_space);

ppr_raw_image_error_type ppr_raw_image_convert (ppr_raw_image_type *image,
                                                ppr_raw_image_color_space_type new_color_space);

void ppr_raw_image_free (ppr_raw_image_type image);

char* ppr_raw_image_error_message (ppr_raw_image_error_type error);

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* __PITTPATT_RAW_IMAGE_H */
