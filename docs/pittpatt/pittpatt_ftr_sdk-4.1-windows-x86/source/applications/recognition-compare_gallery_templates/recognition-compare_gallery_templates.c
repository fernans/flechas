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
 * EXAMPLE PROGRAM : Compare Galleries
 *
 * This example reads a target gallery and a query gallery from disk and does a
 * gallery-to-gallery comparison.  The result of comparing the two galleries
 * is a similarity matrix that shows how well each template in the target
 * gallery matches each template in the query gallery.
 *
 * A directory is created for each template in the query gallery.  The directory
 * contains a thumbnail for the query template.  It also contains a subdirectory
 * of thumbnails from the target templates that best match the query template.
 *
 * The filenames of the target template thumbnails have the form
 * 'rxx_idyyyy_sz.zzz.jpg' where xx is the rank order, yyyy is the id of the
 * template in the target gallery, and z.zzz is the score for how well that
 * template matches the query template.
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
  int i;                                                 /* Query template loop index                            */
  int j;                                                 /* Target template loop index                           */

  parameters_struct params;                              /* Parameters from command-line                         */
  int i_first_file;                                      /* Index of first file name on command line             */
  
  char *query_gallery_file_name;                         /* Name of query gallery file from command line         */
  char *target_gallery_file_name;                        /* Name of target gallery file from command line        */
  char *output_directory;                                /* Name of results directory from command line          */
  
  ppr_context_type context;                              /* SDK detection context                                */
  ppr_gallery_type query_gallery;                        /* Gallery being compared                               */
  ppr_gallery_type target_gallery;                       /* Gallery being compared to                            */
  ppr_template_id_list_type query_template_id_list;      /* List of template IDs in query gallery                */

  ppr_similarity_matrix_type similarity_matrix;          /* Match scores between templates in each gallery       */
  ppr_similarity_matrix_type adjusted_similarity_matrix; /* Similarity matrix with transitive matching applied   */
  ppr_similarity_matrix_type *p_similarity_matrix;       /* Pointer to similarity matrix to process              */

  ppr_template_id_list_type ranked_id_list;              /* Ranked list of IDs of matching templates             */
  ppr_score_list_type score_list;                        /* Scores corresponding to elements in list             */

  char thumbnail_file_name[FILENAME_MAX];                /* Name of thumbnail file being written                 */ 
  char query_template_directory[FILENAME_MAX];           /* Name of directory created for current query template */
  char ranked_templates_directory[FILENAME_MAX];         /* Name of directory for ranked target templates        */

  /****************************/
  /* Configure and Initialize */
  /****************************/

  sdk_profile_timers_init();
  sdk_profile_timers_name_timer(0, "total");
  sdk_profile_timers_name_timer(1, "initialization");
  sdk_profile_timers_name_timer(2, "read galleries");
  sdk_profile_timers_name_timer(3, "compare galleries");
  sdk_profile_timers_name_timer(4, "transitive matching");
  sdk_profile_timers_name_timer(5, "write results");
  sdk_profile_timers_name_timer(6, "finalize");
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
  TRY(ppr_read_gallery(context, query_gallery_file_name, &query_gallery));
  sdk_profile_timers_stop_timer(2);

  printf("Reading target gallery (%s) from disk...\n", target_gallery_file_name);

  sdk_profile_timers_start_timer(2);
  TRY(ppr_read_gallery(context, target_gallery_file_name, &target_gallery));
  sdk_profile_timers_stop_timer(2);

  /*********************/
  /* Compare Galleries */
  /*********************/

  /* Take two galleries and compute a template<->template similarity matrix */
  printf("Comparing galleries...\n");
  sdk_profile_timers_start_timer(3);
  TRY(ppr_compare_galleries(context, query_gallery, target_gallery, &similarity_matrix));
  sdk_profile_timers_stop_timer(3);

  /* Apply transitive matching if requested */
  if (params.use_transitive_matching)
  {
    sdk_profile_timers_start_timer(4);
    ppr_apply_transitive_matching(context, similarity_matrix, &adjusted_similarity_matrix);
    sdk_profile_timers_stop_timer(4);
    p_similarity_matrix = &adjusted_similarity_matrix;
    ppr_free_similarity_matrix(similarity_matrix);
  }
  else
  {
    p_similarity_matrix = &similarity_matrix;
  }

  /********************/
  /* Get Rank Results */
  /********************/
  
  /* Loop through each template in query gallery */
  printf("Getting rank results and writing to disk...\n");
  ppr_get_template_id_list_from_gallery(context, query_gallery, &query_template_id_list);
  sdk_profile_timers_start_timer(5);
  for (i = 0; i < query_template_id_list.num_template_ids; i++)
  {
    /* Make directory for query template */
    sprintf(query_template_directory, "%s/query_template_%04d", output_directory,
            query_template_id_list.template_ids[i]);
    sdk_utils_make_directory(query_template_directory);

    /* Write thumbnail for query template */
    sprintf(thumbnail_file_name, "%s/query_thumbnail.jpg", query_template_directory);
    sdk_utils_write_template_thumbnail_by_id(thumbnail_file_name,
                                             context,
                                             query_gallery,
                                             query_template_id_list.template_ids[i]);

    /* Get ranked list of target templates for current query template */
    TRY(ppr_get_ranked_template_list_for_template(context,
                                                  *p_similarity_matrix,
                                                  query_template_id_list.template_ids[i],
                                                  params.max_ranked_list_length,
                                                  params.ranked_list_threshold,
                                                  &ranked_id_list,
                                                  &score_list));

    /* Make subdirectory for ranked target templates */
    if (ranked_id_list.num_template_ids > 0)
    {
      sprintf(ranked_templates_directory, "%s/ranked_targets", query_template_directory);
      sdk_utils_make_directory(ranked_templates_directory);
    }

    /* Write ranked target template thumbnails */
    for (j = 0; j < ranked_id_list.num_template_ids; j++)
    {
      sprintf(thumbnail_file_name, "%s/r%02d_id%04d_s%.3f.jpg",
              ranked_templates_directory,
              j,
              ranked_id_list.template_ids[j],
              score_list.scores[j]);

      sdk_utils_write_template_thumbnail_by_id(thumbnail_file_name,
                                               context,
                                               target_gallery,
                                               ranked_id_list.template_ids[j]);
    }
      
    ppr_free_template_id_list(ranked_id_list);
    ppr_free_score_list(score_list);
  }
  sdk_profile_timers_stop_timer(5);

  /************/
  /* Finalize */
  /************/
  
  printf("Finalizing...\n");

  sdk_profile_timers_start_timer(6);
  ppr_free_gallery(query_gallery);
  ppr_free_gallery(target_gallery);
  ppr_free_similarity_matrix(*p_similarity_matrix);
  ppr_free_template_id_list(query_template_id_list);

  ppr_release_context(context);
  ppr_finalize_sdk();
  sdk_profile_timers_stop_timer(6);

  printf("Processing complete\n");

  sdk_profile_timers_stop_timer(0);
  sdk_profile_timers_print_timers();

  return EXIT_SUCCESS;
}

/*****************************************************************************/
