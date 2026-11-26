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

#include "final_gallery.h"
#include "reduced_tracks.h"
#include "sdk_utils.h"

/*******************************************************************************
 * Local Function Prototypes
 ******************************************************************************/

static void set_id_array_as_same(ppr_context_type context,
                                 ppr_gallery_type *gallery,
                                 int *template_ids,
                                 int num_template_ids);

static int tracks_overlap(ppr_completed_track_type track_a,
                          ppr_completed_track_type track_b);

/*******************************************************************************
 * Functions
 ******************************************************************************/

/*
 * Adds representative templates from all reduced tracks to a single gallery, using template string to keep track of
 * index of track that each template originated from.  Sets relationship to SAME_SUBJECT for all templates coming from
 * the same track.
 */
void create_final_gallery(ppr_context_type context,
                          int num_representative_templates_per_track,
                          reduced_track_list_type *reduced_track_list,
                          ppr_gallery_type *final_gallery)
{
  int i;                                              /* Index for looping through tracks                          */
  int t;                                              /* Index for looping through template IDs                    */

  int *added_template_ids;                            /* Array of IDs added from current track - used to set SAMEs */
  int num_added_ids;                                  /* Number of IDs in the added ID array                       */
  reduced_track_type *p_trk;                          /* Pointer to current track                                  */
  ppr_template_id_list_type template_id_list;         /* List of template IDs in the current track gallery         */
  ppr_template_type curr_template;                    /* Template being copied from track gallery to final gallery */
  char template_string[PPR_MAX_TEMPLATE_STRING_SIZE]; /* String to embed in current template                       */
  
  /* Create empty gallery */
  TRY(ppr_create_gallery(context, final_gallery));

  /* Allocate array to keep track of IDs added to gallery from each track */
  added_template_ids = (int*)malloc(num_representative_templates_per_track * sizeof(int));
  sdk_utils_check_allocation(added_template_ids, "added_template_ids", "main");

  /* Loop through all tracks */
  for (i = 0; i < reduced_track_list->num_tracks; i++)
  {
    p_trk = &reduced_track_list->tracks[i];

    /* Add all templates from this track to the final gallery */
    TRY(ppr_get_template_id_list_from_gallery(context, p_trk->gallery, &template_id_list));
    
    num_added_ids = 0;
    for (t = 0; t < template_id_list.num_template_ids; t++)
    {
      TRY(ppr_get_template_by_id(context, p_trk->gallery, template_id_list.template_ids[t], &curr_template));
      sprintf(template_string, "%d", i);
      TRY(ppr_set_template_string(context, &curr_template, template_string));
      TRY(ppr_copy_template_to_gallery(context, final_gallery, curr_template, &added_template_ids[num_added_ids]));
      num_added_ids++;
      
      ppr_free_template(curr_template);
    }

    /* Mark all added templates as same subject in final gallery */
    set_id_array_as_same(context, final_gallery, added_template_ids, num_added_ids);
    
    ppr_free_template_id_list(template_id_list);
  }
  
  free(added_template_ids);
}

/******************************************************************************/

/* For subjects that appear together in any video frame, marks their template relationships as DIFFERENT_SUBJECTS */
void set_overlapping_tracks_as_diffs(ppr_context_type context,
                                     ppr_completed_track_list_type completed_track_list,
                                     ppr_gallery_type *final_gallery)
{
  int i;                                              /* Index for looping through templates               */
  int j;                                              /* Index for looping through templates               */
  
  ppr_template_id_list_type template_id_list;         /* List of template IDs in final gallery             */
  char template_string[PPR_MAX_TEMPLATE_STRING_SIZE]; /* String acquired from current template             */
  int track_index_i;                                  /* Index of template i acquired from template string */
  int track_index_j;                                  /* Index of template j acquired from template string */

  TRY(ppr_get_template_id_list_from_gallery(context, *final_gallery, &template_id_list));

  for (i = 0; i < template_id_list.num_template_ids; i++)
  {
    TRY(ppr_get_template_string_by_id(context, *final_gallery, template_id_list.template_ids[i], template_string));
    sscanf(template_string, "%d", &track_index_i);

    for (j = i + 1; j < template_id_list.num_template_ids; j++)
    {
      TRY(ppr_get_template_string_by_id(context, *final_gallery, template_id_list.template_ids[j], template_string));
      sscanf(template_string, "%d", &track_index_j);
      
      if (track_index_j != track_index_i)
      {
        if (tracks_overlap(completed_track_list.completed_tracks[track_index_i],
                           completed_track_list.completed_tracks[track_index_j]))
        {
          TRY(ppr_set_template_relationship(context, final_gallery, template_id_list.template_ids[i],
                                            template_id_list.template_ids[j], PPR_RELATIONSHIP_DIFFERENT_SUBJECTS));
        }
      }
    }
  }

  ppr_free_template_id_list(template_id_list);
}

/******************************************************************************/

/* Compares final clustered gallery to known subject gallery and assigns names to tracks */
void compare_to_known_subject_gallery(ppr_context_type context,
                                      ppr_gallery_type clustered_gallery,
                                      ppr_subject_list_type clustered_subject_list,
                                      ppr_gallery_type known_gallery,
                                      ppr_subject_list_type known_subject_list,
                                      float match_threshold,
                                      reduced_track_list_type *reduced_track_list)
{
  int i;                                              /* Index for looping through clustered subjects                 */
  int t;                                              /* Index for looping through clustered subject template IDs     */
  
  ppr_similarity_matrix_type similarity_matrix;       /* Scores from comparing clustered gallery to known gallery     */
  ppr_index_list_type best_target_indices;            /* Index of known sub that best matches each clustered subject  */
  ppr_score_list_type score_list;                     /* Score of best known subject match for each clustered subject */
  int track_index;                                    /* Index of track to add name to                                */
  
  int target_template_id;                             /* ID of known template to get string from                     */
  char template_string[PPR_MAX_TEMPLATE_STRING_SIZE]; /* String from known template containing subject name          */
  char name[PPR_MAX_TEMPLATE_STRING_SIZE];            /* Subject name from known template string                     */

  /* Compare clustered gallery to known gallery and determine which known subjects best match each clustered subject */
  TRY(ppr_compare_galleries(context, clustered_gallery, known_gallery, &similarity_matrix));
  TRY(ppr_get_best_matching_subjects(context,
                                     similarity_matrix,
                                     clustered_subject_list,
                                     known_subject_list,
                                     0,
                                     &best_target_indices,
                                     &score_list));

  /* Loop through all clustered subjects */
  for (i = 0; i < clustered_subject_list.num_subjects; i++)
  {
    /* Loop through all templates in current clustered subject */
    for (t = 0; t < clustered_subject_list.subjects[i].num_template_ids; t++)
    {
      /* Determine which track this template belongs to */
      TRY(ppr_get_template_string_by_id(context,
                                        clustered_gallery,
                                        clustered_subject_list.subjects[i].template_ids[t],
                                        template_string));
      sscanf(template_string, "%d", &track_index);

      /* If match score meets threshold, assign subject name to track */
      if (score_list.scores[i] > match_threshold)
      {
        target_template_id = known_subject_list.subjects[best_target_indices.indices[i]].template_ids[0];
        TRY(ppr_get_template_string_by_id(context, known_gallery, target_template_id, template_string));
        sdk_utils_get_item_from_template_string(template_string, "#NAME", name);
        if (name[0] == '\0')
          sprintf(reduced_track_list->tracks[track_index].name, "subject_%2d", best_target_indices.indices[i]);
        else
          strncpy(reduced_track_list->tracks[track_index].name, name, MAX_NAME_SIZE - 1);
      }
      else
      {
        sprintf(reduced_track_list->tracks[track_index].name, "unmatched_%03d", i);
      }
    }
  }

  ppr_free_index_list(best_target_indices);
  ppr_free_score_list(score_list);
  ppr_free_similarity_matrix(similarity_matrix);
}

/*******************************************************************************
 * Local Functions
 ******************************************************************************/

/* Given an array of template IDs, marks all relationships between them as SAME_SUBJECT */
static void set_id_array_as_same(ppr_context_type context,
                                 ppr_gallery_type *gallery,
                                 int *template_ids,
                                 int num_template_ids)
{
  int i; /* Index for looping through template IDs */
  int j; /* Index for looping through template IDs */
  
  for (i = 0; i < num_template_ids; i++)
  {
    for (j = i + 1; j < num_template_ids; j++)
    {
      TRY(ppr_set_template_relationship(context, gallery, template_ids[i], template_ids[j],
                                        PPR_RELATIONSHIP_SAME_SUBJECT));
    }
  }
}

/******************************************************************************/

/* Returns 1 if the two tracks appear at the same time in any video frame, 0 otherwise */
static int tracks_overlap(ppr_completed_track_type track_a,
                          ppr_completed_track_type track_b)
{
  int i_track_a; /* Current index into track a */
  int i_track_b; /* Current index into track b */

  /* Tracks don't overlap if their ranges don't overlap                                      */
  /* If their ranges do overlap, the tracks still may not overlap due to gaps within a track */
  if (track_a.track_summary.stop_index < track_b.track_summary.start_index ||
      track_b.track_summary.stop_index < track_a.track_summary.start_index)
  {
    return 0;
  }

  /* Check to see if there exists a frame where both tracks have objects */
  i_track_a = 0;
  i_track_b = 0;
  
  while ((i_track_a < track_a.num_tracked_objects) && (i_track_b < track_b.num_tracked_objects))
  {
    while ((i_track_a < track_a.num_tracked_objects) &&
           (track_a.tracked_objects[i_track_a].index < track_b.tracked_objects[i_track_b].index))
    {
      i_track_a++;
    }

    while ((i_track_b < track_b.num_tracked_objects) &&
           (track_b.tracked_objects[i_track_b].index < track_a.tracked_objects[i_track_a].index))
    {
      i_track_b++;
    }

    if (track_a.tracked_objects[i_track_a].index == track_b.tracked_objects[i_track_b].index)
      return 1;
  }

  /* Even though the track ranges overlap, there is no place where they both have objects in the same frame */
  return 0;
}

/******************************************************************************/
