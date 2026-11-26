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


#ifndef FINAL_GALLERY_H
#define FINAL_GALLERY_H

#include "reduced_tracks.h"

/* If C++ then we need to extern "C". Compiler defines __cplusplus */
#ifdef __cplusplus
extern "C" {
#endif
  
/*******************************************************************************
 * Function Prototypes
 ******************************************************************************/

/*
 * Adds representative templates from all reduced tracks to a single gallery, using template string to keep track of
 * index of track that each template originated from.  Sets relationship to SAME_SUBJECT for all templates coming from
 * the same track.
 */
void create_final_gallery(ppr_context_type context,
                          int num_representative_templates_per_track,
                          reduced_track_list_type *reduced_track_list,
                          ppr_gallery_type *final_gallery);

/* For subjects that appear together in any video frame, marks their template relationships as DIFFERENT_SUBJECTS */
void set_overlapping_tracks_as_diffs(ppr_context_type context,
                                     ppr_completed_track_list_type completed_track_list,
                                     ppr_gallery_type *final_gallery);

/* Compares final clustered gallery to known subject gallery and assigns names to tracks */
void compare_to_known_subject_gallery(ppr_context_type context,
                                      ppr_gallery_type final_gallery,
                                      ppr_subject_list_type final_subject_list,
                                      ppr_gallery_type known_gallery,
                                      ppr_subject_list_type known_subject_list,
                                      float match_threshold,
                                      reduced_track_list_type *reduced_track_list);

/******************************************************************************/
  
#ifdef __cplusplus
}
#endif

#endif /* FINAL_GALLERY_H */
