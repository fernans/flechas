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


#ifndef ACQUIRE_TRACKS_H
#define ACQUIRE_TRACKS_H

#include "pittpatt_ftr_sdk.h"

#include "reduced_tracks.h"

/* If C++ then we need to extern "C". Compiler defines __cplusplus */
#ifdef __cplusplus
extern "C" {
#endif
  
/*******************************************************************************
 * Function Prototypes
 ******************************************************************************/  

/* Given the name of a video, submits the entire video to the tracker, resulting in a completed track list */
void get_completed_tracks_from_video(ppr_context_type context,
                                     char *file_name,
                                     ppr_completed_track_list_type *completed_track_list);

/* Given a completed track list, selects temporally distributed frontal objects from each track */
void get_dispersed_frontal_objects_from_all_tracks(ppr_context_type context,
                                                   ppr_completed_track_list_type completed_track_list,
                                                   int max_dispersed_objects_per_track,
                                                   reduced_track_list_type *reduced_track_list);

/* Given a list of temporally distributed tracks, loops through the entire video, extracting representative templates */
void trim_video_tracks_to_representative_templates(ppr_context_type context,
                                                   char *file_name,
                                                   int num_representative_templates,
                                                   reduced_track_list_type *reduced_track_list);
  
/******************************************************************************/
  
#ifdef __cplusplus
}
#endif

#endif /* ACQUIRE_TRACKS_H */
