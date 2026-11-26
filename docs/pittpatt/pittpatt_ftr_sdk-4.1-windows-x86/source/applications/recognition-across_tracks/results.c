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
#include <string.h>

#include "pittpatt_ftr_sdk.h"
#include "pittpatt_raw_image_io.h"
#include "pittpatt_video_io.h"

#include "results.h"
#include "reduced_tracks.h"
#include "sdk_utils.h"

/*******************************************************************************
 * Constants
 ******************************************************************************/

#define BIT_RATE 1000

/*******************************************************************************
 * Functions
 ******************************************************************************/

/* Assigns a color to each track based on which subject it is a part of */
void assign_track_colors(ppr_context_type context,
                         ppr_gallery_type final_gallery,
                         ppr_subject_list_type final_subject_list,
                         int use_known_gallery,
                         reduced_track_list_type *reduced_track_list)
{
  int i;                                              /* Index for looping through subjects                       */
  int t;                                              /* Index for looping though template IDs in current subject */
                                                     
  sdk_draw_utils_color_type color;                    /* Color to use for current subject                         */
  char template_string[PPR_MAX_TEMPLATE_STRING_SIZE]; /* String acquired from current template in subject         */
  int track_index;                                    /* Index of track taken from template string                */
  
  for (i = 0; i < final_subject_list.num_subjects; i++)
  {
    color = i % SDK_DRAW_UTILS_NUM_COLORS;
    for (t = 0; t < final_subject_list.subjects[i].num_template_ids; t++)
    {
      TRY(ppr_get_template_string_by_id(context, final_gallery, final_subject_list.subjects[i].template_ids[t],
                                        template_string));
      sscanf(template_string, "%d", &track_index);
      reduced_track_list->tracks[track_index].i_subject = i;

      /* When known gallery is provided, tracks of same subject are identified by name instead of color */
      if (use_known_gallery)
        reduced_track_list->tracks[track_index].color = SDK_DRAW_UTILS_GREEN;
      else
        reduced_track_list->tracks[track_index].color = color;
      
      reduced_track_list->tracks[track_index].line_type = SDK_DRAW_UTILS_SOLID_LINE;
    }
  }
}

/******************************************************************************/

/* Overlays completed tracks onto video frames, using colors and line types from reduced track list */
void overlay_completed_track_list_on_video(const char *in_filename,
                                           ppr_completed_track_list_type completed_track_list,
                                           reduced_track_list_type reduced_track_list,
                                           int overlay_frame_numbers,
                                           const char *output_directory,
                                           const char *output_video_format)
{
  int i;                                               /* Index for looping through tracks               */
  int frame_num;                                       /* Frame number of current frame in video         */

  ppr_video_io_type input_video;                       /* Video being processed                          */
  ppr_video_io_type output_video;                      /* Video being written                            */
  int frame_rate_numerator;                            /* Numerator value in input video frame rate      */
  int frame_rate_denominator;                          /* Denominator value in input video frame rate    */
  int *i_tracked_object_lists;                         /* Index into object list for each track          */

  ppr_raw_image_type raw_image;                        /* Current frame from video                       */
  char title[FILENAME_MAX];                            /* Name of input video without extension and path */
  char out_filename[FILENAME_MAX];                     /* Name of current frame to output                */

  char overlay_string[64];                             /* Subject index and tracked ID overlay string    */
  char clustered_subject_string[32];                   /* String giving index of clustered subject       */
  sdk_draw_utils_line_attributes_type line_attributes; /* Attributes for drawing overlays                */

  /* Initialize attributes for overlays */
  sdk_draw_utils_init_line_attributes(&line_attributes);

  /* Open input video */
  TRY_VIDEO(ppr_video_io_open(&input_video, in_filename));
  TRY_VIDEO(ppr_video_io_get_frame_rate(input_video, &frame_rate_numerator, &frame_rate_denominator));

  /* Create output video */
  sdk_utils_get_file_title(in_filename, title);
  sprintf(out_filename, "%s/%s_overlay.%s", output_directory, title, output_video_format);
  TRY_VIDEO(ppr_video_io_create(&output_video, out_filename, BIT_RATE, frame_rate_numerator, frame_rate_denominator));

  /* Create array of indices into tracked object arrays */
  i_tracked_object_lists = (int*)malloc(completed_track_list.num_completed_tracks * sizeof(int));
  sdk_utils_check_allocation(i_tracked_object_lists, "i_tracked_object_lists", __FUNCTION__);
  for (i = 0; i < completed_track_list.num_completed_tracks; i++)
    i_tracked_object_lists[i] = 0;

  /* Loop through each frame of input video */
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
        sprintf(overlay_string, "track_id=%d",
                completed_track_list.completed_tracks[i].tracked_objects[i_tracked_object_lists[i]].tracked_id);

        /* Add name or subject ID if recognizable track */
        if (reduced_track_list.tracks[i].i_subject >= 0)
        {
          strcat(overlay_string, "\n");
          
          /* Add name to string if one exists, else use clustered subject ID */
          if (reduced_track_list.tracks[i].name[0] != '\0')
          {
            strcat(overlay_string, reduced_track_list.tracks[i].name); 
          }
          else
          {
            sprintf(clustered_subject_string, "subj_index=%d", reduced_track_list.tracks[i].i_subject);
            strcat(overlay_string, clustered_subject_string);
          }
        }
        
        /* Overlay polygon for this object */
        line_attributes.color = reduced_track_list.tracks[i].color;
        line_attributes.line_type = reduced_track_list.tracks[i].line_type;
        line_attributes.thickness = 3;
        sdk_utils_overlay_object(
            &raw_image,
            completed_track_list.completed_tracks[i].tracked_objects[i_tracked_object_lists[i]].object,
            overlay_string,
            line_attributes);
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

/******************************************************************************/
