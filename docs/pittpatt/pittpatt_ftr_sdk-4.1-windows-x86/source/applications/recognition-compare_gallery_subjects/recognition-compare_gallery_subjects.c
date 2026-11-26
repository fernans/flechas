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
 * EXAMPLE PROGRAM : Compare To Known Subjects
 *
 * This example compares a query gallery to a known subject gallery and
 * determines which subjects in the known subject gallery best match each
 * subject in the query gallery.
 *
 * A directory is created for each subject in the query gallery.  The directory
 * contains a thumbnail for each template in the query subject.  It also
 * contains a subdirectory of target subjects that best match the query subject.
 * Each target subject is its own directory, consisting of thumbnails for each
 * template in that target subject.
 *
 * The filenames of the target subject directories have the form
 * 'rxx_iyyyy_sz.zzz' where xx is the rank order, yyyy is the index of the
 * target subject, and z.zzz is the score of how well the query subject matches
 * the target subject.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pittpatt_ftr_sdk.h"
#include "pittpatt_license.h"

#include "pittpatt_raw_image_io.h"

#include "parse_command_line.h"
#include "sdk_utils.h"
#include "sdk_profile_timers.h"

/*****************************************************************************
 * Functions
 *****************************************************************************/

int main (int argc, char *argv[])
{
  int i;                                        /* Query subject loop index                                */
  int j;                                        /* Target subhect loop index                               */

  parameters_struct params;                     /* Parameters from command-line                            */
  int i_first_file;                             /* Index of first file name on command line                */

  char *query_gallery_file_name;                /* Name of query gallery file from command line            */
  char *target_gallery_file_name;               /* Name of target gallery file from command line           */
  char *output_directory;                       /* Name of results directory from command line             */
  
  ppr_context_type context;                     /* SDK detection context                                   */
  ppr_gallery_type query_gallery;               /* Gallery being compared                                  */
  ppr_subject_list_type query_subject_list;     /* Subject list of gallery being compared                  */
  ppr_gallery_type target_gallery;              /* Gallery being compared to                               */
  ppr_subject_list_type target_subject_list;    /* Subject list of gallery being compared to               */

  ppr_similarity_matrix_type similarity_matrix; /* Match scores between templates in each gallery          */

  ppr_index_list_type ranked_index_list;        /* Ranked list of indices of matching subjects             */
  ppr_score_list_type score_list;               /* Scores corresponding to elements in lists               */

  char query_subject_directory[FILENAME_MAX];   /* Name of directory created for current query subject     */
  char ranked_subjects_directory[FILENAME_MAX]; /* Name of directory for ranked target subjects            */
  char target_subject_directory[FILENAME_MAX];  /* Name of directory created for target subject thumbnails */

  /****************************/
  /* Configure and Initialize */
  /****************************/

  sdk_profile_timers_init();
  sdk_profile_timers_name_timer(0, "total");
  sdk_profile_timers_name_timer(1, "initialization");
  sdk_profile_timers_name_timer(2, "read galleries");
  sdk_profile_timers_name_timer(3, "compare galleries");
  sdk_profile_timers_name_timer(4, "write results");
  sdk_profile_timers_name_timer(5, "finalize");
  sdk_profile_timers_start_timer(0);

  /* Get command-line options */
  i_first_file = parse_command_line(argc, argv, &params);
  query_gallery_file_name = argv[i_first_file];
  target_gallery_file_name = argv[i_first_file + 1];
  output_directory = argv[i_first_file + 2];

  /* Create output directory */
  sdk_utils_make_directory(output_directory);
  printf("\n");
  printf("Created %s directory for output\n", output_directory);

  /* Get context and initialize */
  context = ppr_get_context();
  TRY(ppr_enable_recognition(context));

  TRY(ppr_set_license(context, my_license_id, my_license_key));
  TRY(ppr_set_models_path(context, params.models_path));
  TRY(ppr_set_num_recognition_threads(context, params.num_threads));

  printf("Initializing...\n");
  
  sdk_profile_timers_start_timer(1);
  TRY(ppr_initialize_context(context));
  sdk_profile_timers_stop_timer(1);

  /*************************************/
  /* Load Previously Created Galleries */
  /*************************************/

  printf("Reading query gallery (%s) from disk...\n", query_gallery_file_name);

  sdk_profile_timers_start_timer(2);
  TRY(ppr_read_gallery_with_subject_list(context, query_gallery_file_name, &query_gallery, &query_subject_list));
  sdk_profile_timers_stop_timer(2);

  if (query_subject_list.num_subjects == 0)
  {
    printf("Query gallery does not have an associated subject list\n");
    printf("Creating one from the query gallery\n");
    ppr_free_subject_list(query_subject_list);
    TRY(ppr_cluster_gallery(context, query_gallery, 0, &query_subject_list));
  }

  printf("Reading known subject gallery (%s) from disk...\n", target_gallery_file_name);

  sdk_profile_timers_start_timer(2);
  TRY(ppr_read_gallery_with_subject_list(context, target_gallery_file_name, &target_gallery, &target_subject_list));
  sdk_profile_timers_stop_timer(2);

  if (target_subject_list.num_subjects > 0)
  {
    printf("\n");
    printf("Known Subject List:\n");
    printf("\n");
    sdk_utils_print_subject_list(stdout, target_subject_list, 1);
    printf("\n");
  }
  else
  {
    fprintf(stderr, "ERROR: known subject gallery (%s) does not have a subject list\n", target_gallery_file_name);
    fprintf(stderr, "  use recognition-create_known_subjects to create a known subject gallery\n");
    exit(EXIT_FAILURE);
  }

  /*********************/
  /* Compare Galleries */
  /*********************/
  
  /* Take two galleries and compute a template<->template similarity matrix */
  printf("Comparing galleries...\n");
  sdk_profile_timers_start_timer(3);
  TRY(ppr_compare_galleries(context, query_gallery, target_gallery, &similarity_matrix));
  sdk_profile_timers_stop_timer(3);

  /********************/
  /* Get Rank Results */
  /********************/
  
  /* Loop through each subject in query subject list */
  printf("Getting rank results and writing to disk...\n");
  sdk_profile_timers_start_timer(4);
  for (i = 0; i < query_subject_list.num_subjects; i++)
  {
    /* Make directory for query subject and write all query subject thumbnails to it */
    sprintf(query_subject_directory, "%s/query_subject_%04d", output_directory, i);    
    sdk_utils_write_template_id_list_thumbnails(
        query_subject_directory,
        context,
        query_gallery,
        1,
        query_subject_list.subjects[i]);

    /* Get ranked list of target subjects for current query subject */
    TRY(ppr_get_ranked_subject_list_for_subject(context,
                                                similarity_matrix,
                                                query_subject_list.subjects[i],
                                                target_subject_list,
                                                params.max_ranked_list_length,
                                                params.ranked_list_threshold,
                                                &ranked_index_list,
                                                &score_list));

    /* Make subdirectory for ranked target subjects */
    if (ranked_index_list.num_indices > 0)
    {
      sprintf(ranked_subjects_directory, "%s/ranked_subjects", query_subject_directory);
      sdk_utils_make_directory(ranked_subjects_directory);
    }

    /* Write ranked target subject thumbnails */
    for (j = 0; j < ranked_index_list.num_indices; j++)
    {
      sprintf(target_subject_directory, "%s/r%02d_i%04d_s%.3f",
              ranked_subjects_directory,
              j,
              ranked_index_list.indices[j],
              score_list.scores[j]);
      
      sdk_utils_write_template_id_list_thumbnails(
          target_subject_directory,
          context,
          target_gallery,
          1,
          target_subject_list.subjects[ranked_index_list.indices[j]]);
    }
    
    ppr_free_index_list(ranked_index_list);
    ppr_free_score_list(score_list);
  }
  sdk_profile_timers_stop_timer(4);

  /************/
  /* Finalize */
  /************/

  printf("Finalizing...\n");

  sdk_profile_timers_start_timer(5);
  ppr_free_gallery(query_gallery);
  ppr_free_gallery(target_gallery);
  ppr_free_subject_list(query_subject_list);
  ppr_free_subject_list(target_subject_list);
  ppr_free_similarity_matrix(similarity_matrix);

  ppr_release_context(context);
  ppr_finalize_sdk();
  sdk_profile_timers_stop_timer(5);

  printf("Processing complete\n");

  sdk_profile_timers_stop_timer(0);
  sdk_profile_timers_print_timers();

  return EXIT_SUCCESS;
}

/*****************************************************************************/
