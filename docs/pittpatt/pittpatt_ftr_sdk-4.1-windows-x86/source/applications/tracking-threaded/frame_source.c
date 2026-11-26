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


/*******************************************************************************
 *
 * file   : frame_source.c
 * date   : Thu Jan  7 11:03:03 2010
 *
 * see header file for details
 *
 * Copyright (C) 2010 Pittsburgh Pattern Recognition, Inc.
 * All Rights Reserved.
 *
 ******************************************************************************/
#include "frame_source.h"
#include <stdlib.h>
#include <string.h>

frame_source_type frame_source_open(char *input_file_name)
{
  char extension[100];   
  
  sdk_utils_get_file_extension(input_file_name, extension);
  if (strcmp(extension, "txt") == 0)
    return open_filelist(input_file_name);
  else
    return open_video(input_file_name);
}

frame_source_type open_shared(char *input_file_name)
{
  frame_source_type answer;
  pthread_mutex_init(&answer.input_mutex, NULL);
  strcpy(answer.input_file_name, input_file_name);
  answer.frame_num = 0;
  return answer;
}

frame_source_type open_video(char *input_file_name)
{
  frame_source_type answer;
  answer = open_shared(input_file_name);
  answer.using_video = 1;

  TRY_VIDEO(ppr_video_io_open(&answer.video, input_file_name));
      
  return answer;
}

frame_source_type open_filelist(char *input_file_name)
{
  frame_source_type answer;
  answer = open_shared(input_file_name);
  answer.using_video = 0;

  sdk_utils_read_file_list(input_file_name, &answer.image_list);

  return answer;
}


void frame_source_reset(frame_source_type *src)
{
  src->frame_num = 0;
  if (src->using_video)
  {
    TRY_VIDEO(ppr_video_io_close(src->video));
    TRY_VIDEO(ppr_video_io_open(&src->video, src->input_file_name));
  }
}

void frame_source_close(frame_source_type *src)
{
  if (src->using_video)
  {
    TRY_VIDEO(ppr_video_io_close(src->video));
  }
  else
  {
    sdk_utils_free_file_list(src->image_list);
  }

}

ppr_image_type get_next_frame(frame_source_type *src, int *out_frame_num)
{
  ppr_raw_image_type raw_image;
  ppr_image_type image;
  
  *out_frame_num = -1;
  raw_image.bytes_per_line = 0;
  raw_image.data = 0;

  pthread_mutex_lock(&src->input_mutex);
  
  if (src->using_video)
  {
    if (ppr_video_io_is_frame_available(src->video))
    {
      TRY_VIDEO(ppr_video_io_get_frame(src->video, &raw_image));
      TRY_VIDEO(ppr_video_io_step_forward(src->video));
      *out_frame_num = src->frame_num++;
      TRY(ppr_create_image(raw_image, &image));
    }
  }
  else
  {
    if (src->frame_num < src->image_list.size)
    {
      TRY_RAW_IMAGE(ppr_raw_image_io_read(src->image_list.filenames[src->frame_num], &raw_image));
      *out_frame_num = src->frame_num++;
      TRY(ppr_create_image(raw_image, &image));
    }
  }
  
  pthread_mutex_unlock(&src->input_mutex);
  
  ppr_raw_image_free(raw_image);
  
  return image;
}
