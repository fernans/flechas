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

#include "reduced_tracks.h"
#include "sdk_utils.h"

/*******************************************************************************
 * Functions
 ******************************************************************************/

/* Initializes a structure that will hold a reduced version of a completed track */
void reduced_track_create(ppr_context_type context, reduced_track_type *reduced_track)
{
  TRY(ppr_create_gallery(context, &reduced_track->gallery));
  reduced_track->i_curr_object = 0;
  reduced_track->finalized = 0;
  reduced_track->i_subject = -1;
  reduced_track->name[0] = '\0';
  reduced_track->color = SDK_DRAW_UTILS_GRAY;
  reduced_track->line_type = SDK_DRAW_UTILS_DOTTED_LINE;
}

/******************************************************************************/

/* Frees data within a reduced track structure */
void reduced_track_free(reduced_track_type *reduced_track)
{
  ppr_free_tracked_object_list(reduced_track->dispersed_object_list);
  ppr_free_gallery(reduced_track->gallery);
}

/******************************************************************************/

/* Creates a list of reduced track structures */
void reduced_track_list_create(ppr_context_type context, reduced_track_list_type *reduced_track_list, int num_tracks)
{
  int i; /* Index for looping through reduced tracks */
  
  reduced_track_list->num_tracks = num_tracks;
  reduced_track_list->tracks = (reduced_track_type*)malloc(num_tracks * sizeof(reduced_track_type));
  sdk_utils_check_allocation(reduced_track_list->tracks, "reduced_track_list->tracks", __FUNCTION__);

  for (i = 0; i < num_tracks; i++)
    reduced_track_create(context, &reduced_track_list->tracks[i]);
}

/******************************************************************************/

/* Frees all reduced tracks in a reduced track list */
void reduced_track_list_free(reduced_track_list_type *reduced_track_list)
{
  int i; /* Index for looping through reduced tracks */
  
  for (i = 0; i < reduced_track_list->num_tracks; i++)
    reduced_track_free(&reduced_track_list->tracks[i]);
  free(reduced_track_list->tracks);
}

/******************************************************************************/
