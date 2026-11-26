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


#ifndef __PITTPATT_VIDEO_IO_H 
#define __PITTPATT_VIDEO_IO_H

#include "pittpatt_raw_image.h"

/* If C++ then we need to __extern "C". Compiler defines __cplusplus */
#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Basic Types
 *****************************************************************************/

typedef struct ppr_video_io_struct* ppr_video_io_type;

/*****************************************************************************
 * Enumerated Types
 *****************************************************************************/

typedef enum
{
  PPR_VIDEO_IO_QUIET,
  PPR_VIDEO_IO_VERBOSE
} ppr_video_io_logging_type;

typedef enum
{
  PPR_VIDEO_IO_FALSE,
  PPR_VIDEO_IO_TRUE
} ppr_video_io_boolean_type;

typedef enum
{
  PPR_VIDEO_IO_SUCCESS,
  PPR_VIDEO_IO_FAILURE,
  PPR_VIDEO_IO_INVALID_ARGUMENT,
  PPR_VIDEO_IO_OUT_OF_MEMORY,
  PPR_VIDEO_IO_UNSUPPORTED_FORMAT,
  PPR_VIDEO_IO_UNSUPPORTED_CODEC,
  PPR_VIDEO_IO_NO_MORE_FRAMES,
  PPR_VIDEO_IO_DECODING_ERROR,
  PPR_VIDEO_IO_SEEK_ERROR,
  PPR_VIDEO_IO_FILE_NOT_FOUND,
  PPR_VIDEO_IO_ENCODING_ERROR,
  PPR_VIDEO_IO_REQUIRES_INPUT_VIDEO,
  PPR_VIDEO_IO_REQUIRES_OUTPUT_VIDEO
} ppr_video_io_error_type;
  
/*****************************************************************************
 * Initialization Functions
 *****************************************************************************/

ppr_video_io_error_type ppr_video_io_set_logging_type (ppr_video_io_logging_type logging_type);

ppr_video_io_error_type ppr_video_io_open (ppr_video_io_type *video,
                                           const char *file_name);

ppr_video_io_error_type ppr_video_io_create (ppr_video_io_type *video,
                                             const char *file_name,
                                             int bit_rate_kbps,
                                             int frame_rate_numerator,
                                             int frame_rate_denominator); 

/*****************************************************************************
 * Navigation Functions
 *****************************************************************************/

ppr_video_io_boolean_type ppr_video_io_is_frame_available (ppr_video_io_type video);

ppr_video_io_error_type ppr_video_io_get_frame_index (ppr_video_io_type video,
                                                      int *frame_index);

ppr_video_io_error_type ppr_video_io_step_forward (ppr_video_io_type video);

/*****************************************************************************
 * Frame Extraction Functions
 *****************************************************************************/

ppr_video_io_error_type ppr_video_io_get_frame (ppr_video_io_type video,
                                                ppr_raw_image_type *frame);

/*****************************************************************************
 * Frame Encoding Functions
 *****************************************************************************/

ppr_video_io_error_type ppr_video_io_add_frame (ppr_video_io_type video,
                                                ppr_raw_image_type frame);

/*****************************************************************************
 * Miscellanous Functions
 *****************************************************************************/

ppr_video_io_error_type ppr_video_io_get_frame_rate (ppr_video_io_type video,
                                                     int *frame_rate_numerator,
                                                     int *frame_rate_denominator);

char* ppr_video_io_error_message (ppr_video_io_error_type error);
  
/*****************************************************************************
 * Finalize Functions
 *****************************************************************************/

ppr_video_io_error_type ppr_video_io_close (ppr_video_io_type video);

/*****************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* __PITTPATT_VIDEO_IO_H */ 
