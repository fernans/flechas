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

#include "parse_command_line.h"
#include "sdk_utils.h"
#include "sdk_cmd_line_utils.h"
#include "models_path.h"

/*****************************************************************************
 * Constants
 *****************************************************************************/

#define DEFAULT_MODELS_PATH             MODELS_PATH
#define DEFAULT_NUM_THREADS             sdk_utils_get_num_cpus()
#define DEFAULT_MAX_RANKED_LIST_LENGTH  100
#define DEFAULT_RANKED_LIST_THRESHOLD   0.5f

/*******************************************************************************
 * Local Function Prototypes
 ******************************************************************************/

static void print_usage_and_exit(const char *exe_name);

/*******************************************************************************
 * Functions
 ******************************************************************************/

/*
 * Sets parameters based on command-line arguments.  Uses defaults for any
 * parameters that are not specified.
 */
int parse_command_line(int argc, char *argv[], parameters_struct *params)
{
  int i_first_file;

  char *models_path_str = NULL;

  int help;

  /* Set default parameters */
  strcpy(params->models_path, DEFAULT_MODELS_PATH);
  params->num_threads = DEFAULT_NUM_THREADS;
  params->max_ranked_list_length = DEFAULT_MAX_RANKED_LIST_LENGTH;
  params->ranked_list_threshold = DEFAULT_RANKED_LIST_THRESHOLD;
  
  /* Parse command line */
  i_first_file = sdk_cmd_line_utils_parse(argc, argv,
                                          "-models %S", &models_path_str,
                                          "-threads %d", &params->num_threads,
                                          "-max_length %d", &params->max_ranked_list_length,
                                          "-rank_threshold %d", &params->ranked_list_threshold,
                                          "-transitive_matching", &params->use_transitive_matching,
                                          "-help", &help,
                                          (void*)0);
  if (i_first_file < 0)
  {
    fprintf(stderr, "ERROR: %s\n", sdk_cmd_line_utils_error_message(i_first_file));
    print_usage_and_exit(argv[0]);
  }

  if (help)
    print_usage_and_exit(argv[0]);

  if ((argc - i_first_file) != 3)
  {
    fprintf(stderr, "ERROR: two input files and one output directory must be specified\n");
    print_usage_and_exit(argv[0]);
  }

  /* Set models path */
  if (models_path_str != NULL)
  {
    strcpy(params->models_path, models_path_str);
    free(models_path_str);
  }

  return i_first_file;
}

/*******************************************************************************
 * Local Functions
 ******************************************************************************/

/*
 * Prints the program's usage message and then exits.
 */
static void print_usage_and_exit(const char *exe_name)
{
  fprintf(stderr, "\n");
  fprintf(stderr, "usage: %s [flags] <query gallery> <target gallery> <output directory>\n", exe_name);
  fprintf(stderr, "\n");
  fprintf(stderr, "  where flags are one or more of the following:\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "   -models <string>         :  path to models directory (default = %s)\n", DEFAULT_MODELS_PATH);
  fprintf(stderr, "   -threads <int>           :  number of threads to use (default = number of CPU cores = %d)\n", 
          DEFAULT_NUM_THREADS);
  fprintf(stderr, "   -max_length <int>        :  maximum length of ranked lists (default = %d)\n", 
          DEFAULT_MAX_RANKED_LIST_LENGTH);
  fprintf(stderr, "   -rank_threshold <float>  :  threshold that must be met to add entry to ranked list (default = %.3f)\n", 
          DEFAULT_RANKED_LIST_THRESHOLD);
  fprintf(stderr, "   -transitive_matching     :  apply transitive matching to similarity matrix (use if there are multiple templates of the same subject)\n");
  fprintf(stderr, "   -help                    :  display this usage message and exit\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "example: %s a1_gallery.bin a2_gallery.bin a1_a2_comparison_results\n", exe_name);

  exit(EXIT_FAILURE);
}

/******************************************************************************/
