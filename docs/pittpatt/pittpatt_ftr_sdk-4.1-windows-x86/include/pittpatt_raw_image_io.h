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


#ifndef __PITTPATT_RAW_IMAGE_IO_H 
#define __PITTPATT_RAW_IMAGE_IO_H

#include "pittpatt_raw_image.h"

/* If C++ then we need to __extern "C". Compiler defines __cplusplus */
#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Functions
 *****************************************************************************/

ppr_raw_image_error_type ppr_raw_image_io_read (const char *file_name,
                                                ppr_raw_image_type *image);

ppr_raw_image_error_type ppr_raw_image_io_write (const char *file_name,
                                                 ppr_raw_image_type image);

ppr_raw_image_error_type ppr_raw_image_io_write_with_quality (const char *file_name,
                                                              ppr_raw_image_type image,
                                                              int quality);


/*****************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* __PITTPATT_RAW_IMAGE_IO_H */
