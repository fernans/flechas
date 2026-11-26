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


#ifndef RESULTS_H
#define RESULTS_H

#include "pittpatt_ftr_sdk.h"

#include "reduced_tracks.h"

/* If C++ then we need to extern "C". Compiler defines __cplusplus */
#ifdef __cplusplus
extern "C" {
#endif
  
/*******************************************************************************
 * Function Prototypes
 ******************************************************************************/  

/* Assigns a color to each track based on which subject it is a part of */
void assign_track_colors(ppr_context_type context,
                         ppr_gallery_type final_gallery,
                         ppr_subject_list_type final_subject_list,
                         int use_known_gallery,
                         reduced_track_list_type *reduced_track_list);

/* Overlays completed tracks onto video frames, using colors and line types from reduced track list */
void overlay_completed_track_list_on_video(const char *in_filename,
                                           ppr_completed_track_list_type completed_track_list,
                                           reduced_track_list_type reduced_track_list,
                                           int overlay_frame_numbers,
                                           const char *output_directory,
                                           const char *output_image_format);
  
/******************************************************************************/
  
#ifdef __cplusplus
}
#endif

#endif /* RESULTS_H */
