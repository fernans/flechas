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
 * EXAMPLE : Recognition Across Tracks
 *
 * This example uses the tracker to process a video sequence.  It then selects
 * objects from each track such that they are spaced out over the entire track.
 * It produces templates from those objects and chooses a select few templates
 * to represent each track.  Finally, all resulting templates are clustered to
 * determine which tracks belong to the same subject.
 *
 * If a known-subject gallery is provided, the final gallery is matched against
 * it to assign names to subjects.  Use recognition-create_subject_gallery to
 * create a known-subject gallery.
 *
 * An output video is created with boxes around the tracked faces.  Track ID
 * information is displayed below each box along with the subject's name if it
 * was matched to a subject in the known-subject gallery.
 *
 * Use the -help flag for usage information.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pittpatt_ftr_sdk.h"
#include "pittpatt_license.h"

#include "acquire_tracks.h"
#include "final_gallery.h"
#include "parse_command_line.h"
#include "reduced_tracks.h"
#include "results.h"
#include "sdk_utils.h"

/*****************************************************************************
 * Constants
 *****************************************************************************/

#define OUTPUT_VIDEO_FORMAT "mp4"

/*****************************************************************************
 * Functions
 *****************************************************************************/

int main(int argc, char *argv[])
{
  parameters_struct params;                           /* Parameters from command-line                       */
  int i_first_file;                                   /* Index of first file name on command line           */
  char *video_name;                                   /* Name of video specified on command line            */
  char *output_directory;                             /* Name of output directory specified on command line */

  ppr_context_type context;                           /* SDK context handle                                 */
  ppr_completed_track_list_type completed_track_list; /* List of all tracks found                           */
  reduced_track_list_type reduced_track_list;         /* List of reduced versions of all tracks             */
  
  ppr_gallery_type final_gallery;                     /* Gallery for clustering representative templates    */
  ppr_subject_list_type final_subject_list;           /* Subject list produced by clustering gallery        */
  ppr_gallery_type known_gallery;                     /* Gallery containing known subject templates         */
  ppr_subject_list_type known_subject_list;           /* List of known subjects to assign to tracks         */
  
  char file_name[FILENAME_MAX];                       /* Temporary string for creating file names           */
  
  /****************************/
  /* Configure and Initialize */
  /****************************/

  i_first_file = parse_command_line(argc, argv, &params);
  video_name = argv[i_first_file];
  output_directory = argv[i_first_file + 1];
  
  sdk_utils_make_directory(output_directory);

  context = ppr_get_context();
  TRY(ppr_enable_tracking(context));
  TRY(ppr_enable_recognition(context));
  
  TRY(ppr_set_license(context, my_license_id, my_license_key));
  TRY(ppr_set_models_path(context, params.models_path));
  TRY(ppr_set_detection_precision(context, params.detection_precision));
  TRY(ppr_set_landmark_detector_type(context, params.landmark_detector_type, PPR_AUTOMATIC_LANDMARKS));
  TRY(ppr_set_num_detection_threads(context, params.num_threads));
  TRY(ppr_set_search_pruning_aggressiveness(context, params.search_pruning));
  TRY(ppr_set_num_recognition_threads(context, params.num_threads));
  TRY(ppr_set_tracking_cutoff(context, params.tracking_cutoff));

  if (params.use_visual_attention)
    TRY(ppr_enable_visual_attention(context));

  if (params.use_shot_boundary_detection)
  {
    TRY(ppr_enable_shot_boundary_detection(context));
    TRY(ppr_set_shot_boundary_threshold(context, params.shot_boundary_threshold));
  }

  TRY(ppr_set_min_size(context, params.min_size));
  TRY(ppr_set_max_size(context, params.max_size));
  TRY(ppr_set_frontal_yaw_constraint(context, params.frontal_yaw_constraint));
  TRY(ppr_set_template_extraction_type(context, params.extraction_type));

  sdk_utils_print_context_configuration(stdout, context);

  printf("Initializing...\n");
  
  TRY(ppr_initialize_context(context));

  /*******************/
  /* Main Processing */
  /*******************/
  
  /* Process entire video to get all tracks */
  printf("Getting tracks from video...\n");
  get_completed_tracks_from_video(context, video_name, &completed_track_list);  

  printf("Selecting instances to represent each track and adding to a gallery...\n");
  
  /* Create a list to hold reduced versions of the tracks */
  reduced_track_list_create(context, &reduced_track_list, completed_track_list.num_completed_tracks);

  /* Select temporally distributed frontal objects from each track */
  get_dispersed_frontal_objects_from_all_tracks(context, completed_track_list, params.max_dispersed_objects_per_track,
                                                &reduced_track_list);

  /* Go through video a second time, extracting templates and keeping the best representatives from each track */
  trim_video_tracks_to_representative_templates(context, video_name, params.num_representative_templates,
                                                &reduced_track_list);
  
  /* Create a final gallery, combining representatives from all tracks */
  create_final_gallery(context, params.num_representative_templates, &reduced_track_list, &final_gallery);
  
  /* If two tracks are in the video at the same time, then they are not the same subject */
  set_overlapping_tracks_as_diffs(context, completed_track_list, &final_gallery);
  
  /* Cluster final gallery */
  printf("Clustering gallery...\n");
  TRY(ppr_cluster_gallery(context, final_gallery, params.aggressiveness, &final_subject_list));

  /* Compare clustered gallery to known subject gallery if provided */
  if (params.use_known_gallery)
  {
    printf("Comparing clustered gallery to known gallery and assigning subject names...\n");
    TRY(ppr_read_gallery_with_subject_list(context, params.known_gallery_name, &known_gallery, &known_subject_list));
    compare_to_known_subject_gallery(context,
                                     final_gallery,
                                     final_subject_list,
                                     known_gallery,
                                     known_subject_list,
                                     params.match_threshold,
                                     &reduced_track_list);
    ppr_free_gallery(known_gallery);
    ppr_free_subject_list(known_subject_list);
  }

  /* Assign track colors - tracks of the same subject are assigned the same color if known gallery not provided */
  assign_track_colors(context, final_gallery, final_subject_list, params.use_known_gallery, &reduced_track_list);

  /*****************/
  /* Write Results */
  /*****************/

  /* Write final gallery */
  printf("Writing final gallery to disk...\n");
  sprintf(file_name, "%s/final_gallery.bin", output_directory);
  TRY(ppr_write_gallery_with_subject_list(context, file_name, final_gallery, final_subject_list));
  
  /* Write subject thumbnails */
  printf("Writing thumbnails based on subject list...\n");
  sprintf(file_name, "%s/thumbnails", output_directory);
  sdk_utils_make_directory(file_name);
  sdk_utils_write_subject_list_thumbnails(file_name, context, final_gallery, 0, final_subject_list);

  /* Overlay tracks onto video */
  printf("Overlaying tracks...\n");
  overlay_completed_track_list_on_video(video_name, completed_track_list, reduced_track_list,
                                        params.frame_numbers, output_directory, OUTPUT_VIDEO_FORMAT);
  
  /************/
  /* Finalize */
  /************/

  /* Free allocated data */
  reduced_track_list_free(&reduced_track_list);
  ppr_free_completed_track_list(completed_track_list);
  ppr_free_gallery(final_gallery);
  ppr_free_subject_list(final_subject_list);
  
  /* Release the context since it is no longer needed */
  TRY(ppr_release_context(context));

  /* Finalize the SDK */
  ppr_finalize_sdk();

  printf("Processing complete\n");

  return EXIT_SUCCESS;
}

/******************************************************************************/
