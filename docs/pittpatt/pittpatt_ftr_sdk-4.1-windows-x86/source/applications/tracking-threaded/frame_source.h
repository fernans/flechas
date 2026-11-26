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
 * file   : frame_source.h
 * date   : Thu Jan  7 10:55:45 2010
 * 
 * general frame source (frames from either an image list or a video)
 * 
 * Copyright (C) 2010 Pittsburgh Pattern Recognition, Inc.
 * All Rights Reserved.
*/


#ifndef FRAME_SOURCE_H
#define FRAME_SOURCE_H

#include "sdk_utils.h"
#include "pittpatt_raw_image_io.h"
#include "pittpatt_video_io.h"
#include <pthread.h>

struct frame_source_struct {
  char input_file_name[FILENAME_MAX];
  int using_video;
  int frame_num;
  sdk_utils_file_list_type image_list;
  ppr_video_io_type video;
  pthread_mutex_t input_mutex;
};
typedef struct frame_source_struct frame_source_type;

/* uses filename to determine type of input (txt files are presumed to be image lists) */
frame_source_type frame_source_open(char *input_file_name);

/* don't do "autodetect", force open as a particular type */
frame_source_type open_video(char *input_file_name);
frame_source_type open_filelist(char *input_file_name);


void frame_source_reset(frame_source_type *src);            /* go back to the start */
void frame_source_close(frame_source_type *src);      
ppr_image_type get_next_frame(frame_source_type *src, int *out_frame_num);

#endif
