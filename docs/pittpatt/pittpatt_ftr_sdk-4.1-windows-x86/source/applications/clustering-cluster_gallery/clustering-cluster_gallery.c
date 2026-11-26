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
 * EXAMPLE PROGRAM : Cluster Gallery
 *
 * This example reads a gallery from disk and clusters the gallery using the
 * specified aggressiveness to create a subject list.  A directory structure is
 * created where each directory corresponds to a subject in the subject list,
 * and each subject directory contains thumbnail images extracted from the
 * templates that make up the subject.  The input gallery file is updated to
 * contain the new subject list.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pittpatt_ftr_sdk.h"
#include "pittpatt_license.h"

#include "parse_command_line.h"
#include "sdk_utils.h"
#include "sdk_profile_timers.h"

/*****************************************************************************
 * Functions
 *****************************************************************************/

int main (int argc, char *argv[])
{
  parameters_struct params;           /* Parameters from command-line             */
  int i_first_file;                   /* Index of first file name on command line */

  ppr_context_type context;           /* SDK detection context                    */
  ppr_gallery_type gallery;           /* Gallery read from disk                   */
  ppr_subject_list_type subject_list; /* Subject list read from disk              */

  /****************************/
  /* Configure and Initialize */
  /****************************/

  sdk_profile_timers_init();
  sdk_profile_timers_name_timer(0, "total");
  sdk_profile_timers_name_timer(1, "initialization");
  sdk_profile_timers_name_timer(2, "read gallery");
  sdk_profile_timers_name_timer(3, "cluster gallery");
  sdk_profile_timers_name_timer(4, "write gallery");
  sdk_profile_timers_name_timer(5, "write thumbnails");
  sdk_profile_timers_start_timer(0);

  /* Get command-line options */
  i_first_file = parse_command_line(argc, argv, &params);

  /* Create output directory */
  sdk_utils_make_directory(argv[i_first_file+1]);
  printf("\n");
  printf("Created %s directory for output\n", argv[i_first_file+1]);

  context = ppr_get_context();
  TRY(ppr_enable_recognition(context));

  TRY(ppr_set_license(context, my_license_id, my_license_key));
  TRY(ppr_set_models_path(context, params.models_path));
  TRY(ppr_set_num_recognition_threads(context, params.num_threads));

  printf("Initializing...\n");
  
  sdk_profile_timers_start_timer(1);
  TRY(ppr_initialize_context(context));
  sdk_profile_timers_stop_timer(1);

  /***********************************/
  /* Load Previously Created Gallery */
  /***********************************/

  printf("\n");
  printf("Reading gallery %s from disk...\n", argv[i_first_file]);

  sdk_profile_timers_start_timer(2);
  TRY(ppr_read_gallery(context, argv[i_first_file], &gallery));
  sdk_profile_timers_stop_timer(2);

  printf("\n");
  printf("Gallery:\n");
  printf("\n");
  sdk_utils_print_gallery(stdout, context, gallery, params.print_relationships);

  printf("\n");
  printf("Creating subject list with clustering aggressiveness of %d...\n", params.aggressiveness);

  /* This will have one template per subject if no template relationships are set and aggressiveness is 0 */
  sdk_profile_timers_start_timer(3);
  TRY(ppr_cluster_gallery(context, gallery, params.aggressiveness, &subject_list));
  sdk_profile_timers_stop_timer(3);

  printf("\n");
  printf("Subject List:\n");
  printf("\n");
  sdk_utils_print_subject_list(stdout, subject_list, 1);

  printf("\n");
  printf("Updating gallery file to include subject list...\n");
  sdk_profile_timers_start_timer(4);
  TRY(ppr_write_gallery_with_subject_list(context, argv[i_first_file], gallery, subject_list));
  sdk_profile_timers_stop_timer(4);

  /*********************************/
  /* Write Subject List Thumbnails */
  /*********************************/

  printf("\n");
  printf("Writing thumbnails based on subject list...\n");
  sdk_profile_timers_start_timer(5);
  sdk_utils_write_subject_list_thumbnails(argv[i_first_file+1], context, gallery, 1, subject_list);
  sdk_profile_timers_stop_timer(5);

  /************/
  /* Finalize */
  /************/

  printf("\n");
  printf("Finalizing...\n");

  ppr_free_gallery(gallery);
  ppr_free_subject_list(subject_list);

  ppr_release_context(context);
  ppr_finalize_sdk();

  printf("\n");
  printf("Processing complete\n");

  sdk_profile_timers_stop_timer(0);
  sdk_profile_timers_print_timers();

  return EXIT_SUCCESS;
}

/******************************************************************************/
