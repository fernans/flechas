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


#include <stdio.h>
#include <stdlib.h>

#include "pittpatt_ftr_sdk.h"
#include "pittpatt_raw_image_io.h"
#include "pittpatt_video_io.h"

#include "acquire_tracks.h"
#include "reduced_tracks.h"
#include "sdk_utils.h"

/*****************************************************************************
 * Local Function Prototypes
 *****************************************************************************/

static void advance_indices_to_frame(reduced_track_list_type *reduced_track_list,
                                     int frame_num);

/*******************************************************************************
 * Functions
 ******************************************************************************/

/* Given the name of a video, submits the entire video to the tracker, resulting in a completed track list */
void get_completed_tracks_from_video(ppr_context_type context,
                                     char *file_name,
                                     ppr_completed_track_list_type *completed_track_list)
{
  ppr_video_io_type video;      /* Video being processed                */
  int frame_num;                /* Current frame number (starting at 0) */
  ppr_raw_image_type raw_image; /* Raw video frame                      */
  ppr_image_type image;         /* Image to be submitted to tracker     */
  
  /* Open video */
  TRY_VIDEO(ppr_video_io_open(&video, file_name));

  /* Loop through frames */
  frame_num = 0;
  while (ppr_video_io_is_frame_available(video))
  {
    printf("Processing Frame[%d]\n", frame_num);
      
    /* Read next frame */
    TRY_VIDEO(ppr_video_io_get_frame(video, &raw_image));
      
    /* Convert raw image to pittpatt image type */
    TRY(ppr_create_image(raw_image, &image));
      
    /* Submit image to tracker */
    TRY(ppr_detect_and_track_objects(context, image));

    /* Free image */
    ppr_raw_image_free(raw_image);
    ppr_free_image(image);

    /* Advance to next frame */
    TRY_VIDEO(ppr_video_io_step_forward(video));
    frame_num++;
  }

  /* Free video */
  TRY_VIDEO(ppr_video_io_close(video));

  /* Finalize all tracks internal to the tracker */
  TRY(ppr_finalize_current_tracks(context));

  /* Get all tracks from the tracker */
  TRY(ppr_get_completed_tracks(context, completed_track_list));
}

/******************************************************************************/

/* Given a completed track list, selects temporally distributed frontal objects from each track */
void get_dispersed_frontal_objects_from_all_tracks(ppr_context_type context,
                                                   ppr_completed_track_list_type completed_track_list,
                                                   int max_dispersed_objects_per_track,
                                                   reduced_track_list_type *reduced_track_list)
{
  int i; /* Index for looping through tracks */
  
  for (i = 0; i < reduced_track_list->num_tracks; i++)
  {
    TRY(ppr_get_dispersed_frontal_objects_from_track(context,
                                                     completed_track_list.completed_tracks[i],
                                                     max_dispersed_objects_per_track,
                                                     &reduced_track_list->tracks[i].dispersed_object_list));

    if (reduced_track_list->tracks[i].dispersed_object_list.num_tracked_objects == 0)
      reduced_track_list->tracks[i].finalized = 1;
  }
}

/******************************************************************************/

/* Given a list of temporally distributed tracks, loops through the entire video, extracting representative templates */
void trim_video_tracks_to_representative_templates(ppr_context_type context,
                                                   char *file_name,
                                                   int num_representative_templates,
                                                   reduced_track_list_type *reduced_track_list)
{
  int i;                           /* Index for looking through tracks             */
  
  ppr_video_io_type video;         /* Video being processed                        */
  int frame_num;                   /* Current frame number (starting at 0)         */
  ppr_raw_image_type raw_image;    /* Raw video frame                              */
  ppr_image_type image;            /* Image to be submitted to tracker             */
  ppr_template_type curr_template; /* Current template extracted from track object */
  int id;                          /* ID of template after adding to gallery       */
  reduced_track_type *p_trk;       /* Pointer to current track                     */
  
  /* Open video */
  TRY_VIDEO(ppr_video_io_open(&video, file_name));

  /* Loop through frames */
  frame_num = 0;
  while (ppr_video_io_is_frame_available(video))
  {
    /* Read next frame */
    TRY_VIDEO(ppr_video_io_get_frame(video, &raw_image));
      
    /* Convert raw image to pittpatt image type */
    TRY(ppr_create_image(raw_image, &image));

    /* Generate templates for each object in this frame and add to their respective galleries */
    advance_indices_to_frame(reduced_track_list, frame_num);
    for (i = 0; i < reduced_track_list->num_tracks; i++)
    {
      p_trk = &reduced_track_list->tracks[i];

      if (!p_trk->finalized)
      {
        if (p_trk->dispersed_object_list.tracked_objects[p_trk->i_curr_object].index == frame_num)
        {
          TRY(ppr_extract_template_from_object(
              context,
              image,
              p_trk->dispersed_object_list.tracked_objects[p_trk->i_curr_object].object,
              &curr_template));
          
          TRY(ppr_copy_template_to_gallery(context, &p_trk->gallery, curr_template, &id));
          ppr_free_template(curr_template);
        }
        else if (p_trk->dispersed_object_list.tracked_objects[p_trk->i_curr_object].index < frame_num)
        {
          /* Track is complete */
          TRY(ppr_trim_subject_to_representative_templates(context, &p_trk->gallery, num_representative_templates));
          p_trk->finalized = 1;
        }
      }
    }
    
    /* Free image */
    ppr_raw_image_free(raw_image);
    ppr_free_image(image);

    /* Advance to next frame */
    TRY_VIDEO(ppr_video_io_step_forward(video));
    frame_num++;
  }

  /* Free video */
  TRY_VIDEO(ppr_video_io_close(video));

  /* Trim any remaining tracks that have not been finalized */
  for (i = 0; i < reduced_track_list->num_tracks; i++)
  {
    p_trk = &reduced_track_list->tracks[i];
    
    if (!p_trk->finalized)
    {
      TRY(ppr_trim_subject_to_representative_templates(context, &p_trk->gallery, num_representative_templates));
      p_trk->finalized = 1;
    }
  }
}

/*******************************************************************************
 * Local Functions
 ******************************************************************************/

/* Advances each track until its current object is at or beyond the current frame */
static void advance_indices_to_frame(reduced_track_list_type *reduced_track_list,
                                     int frame_num)
{
  int i;                     /* Index for looping through tracks */
  reduced_track_type *p_trk; /* Pointer to current track         */

  /* Advance tracked object indices until frame_num is reached or passed */
  for (i = 0; i < reduced_track_list->num_tracks; i++)
  {
    p_trk = &reduced_track_list->tracks[i];

    if (p_trk->dispersed_object_list.num_tracked_objects > 0)
    {
      while (
          (p_trk->i_curr_object < p_trk->dispersed_object_list.num_tracked_objects - 1) &&
          (p_trk->dispersed_object_list.tracked_objects[p_trk->i_curr_object].index < frame_num))
      {
        p_trk->i_curr_object++;
      }
    }
  }
}

/******************************************************************************/
