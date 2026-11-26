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


#ifndef REDUCED_TRACKS_H
#define REDUCED_TRACKS_H

#include "pittpatt_ftr_sdk.h"

#include "sdk_utils.h"

/* If C++ then we need to extern "C". Compiler defines __cplusplus */
#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Constants
 ******************************************************************************/

#define MAX_NAME_SIZE 16

/*****************************************************************************
 * Data Types
 *****************************************************************************/

typedef struct
{
  ppr_tracked_object_list_type dispersed_object_list; /* List of temporally distributed frontal objects from track   */
  ppr_gallery_type gallery;                           /* Gallery containing tracks's best representative templates   */
  int i_curr_object;                                  /* Index of current object being processed in object list      */
  int finalized;                                      /* Flag indicating that this track's gallery is complete       */
  int i_subject;                                      /* Index of subject corresponding to this track                */
  char name[MAX_NAME_SIZE];                           /* Name of subject represented by this track                   */
  sdk_draw_utils_color_type color;                    /* Color to use when this track is overlayed onto video        */
  sdk_draw_utils_line_type line_type;                 /* Type of line (dotted or solid) to use when overlaying track */
} reduced_track_type;

typedef struct
{
  int num_tracks;             /* Number of reduced tracks in the list */
  reduced_track_type *tracks; /* Array of reduced track structures    */
} reduced_track_list_type;

/*******************************************************************************
 * Function Prototypes
 ******************************************************************************/

/* Initializes a structure that will hold a reduced version of a completed track */
void reduced_track_create(ppr_context_type context, reduced_track_type *reduced_track);

/* Frees data within a reduced track structure */
void reduced_track_free(reduced_track_type *reduced_track);

/* Creates a list of reduced track structures */
void reduced_track_list_create(ppr_context_type context, reduced_track_list_type *reduced_track_list, int num_tracks);

/* Frees all reduced tracks in a reduced track list */
void reduced_track_list_free(reduced_track_list_type *reduced_track_list);

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* REDUCED_TRACKS_H */
