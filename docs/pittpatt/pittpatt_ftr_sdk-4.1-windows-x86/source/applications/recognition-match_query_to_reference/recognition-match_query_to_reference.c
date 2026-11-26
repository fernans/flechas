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
 * EXAMPLE PROGRAM : Match Query To Reference
 *
 * This is an example of using recognition to match faces between images.  All
 * frontal faces in the query image(s) are compared to all frontal faces in the
 * reference image to determine their similarity.  Scores are reported for each
 * pair of faces.
 *
 * Output images are written with boxes showing the location of the faces that
 * were found in each image.  If a face in a query image matches a face in the
 * reference image, then their boxes will be the same color.  Faces in the query
 * image(s) that do not match any of the faces in the reference image or are
 * not frontal faces are boxed with a dotted line.  Faces in the reference
 * image that are not frontal faces are also boxed with a dotted line.
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
 * Constants
 *****************************************************************************/

#define QUERIES_ARE_UNIQUE           1
#define RESULTS_INDENT               0

#define UNKNOWN_OBJECT_COLOR         SDK_DRAW_UTILS_GRAY
#define UNKNOWN_OBJECT_THICKNESS     2

#define MIN_CONFIDENCE_THRESHOLD     0.0f

#define MEDIUM_CONFIDENCE_THICKNESS  2
#define MEDIUM_CONFIDENCE_THRESHOLD  2.5f

#define HIGH_CONFIDENCE_THICKNESS    4
#define HIGH_CONFIDENCE_THRESHOLD    4.0f

#define REFERENCE_OVERLAY_IMAGE_NAME "reference_image.jpg"
#define QUERY_IMAGE_FORMAT           "jpg"

/*****************************************************************************
 * Local Function Prototypes
 *****************************************************************************/

static void create_gallery_from_image(
  ppr_context_type context,
  ppr_raw_image_type *raw_image,
  ppr_object_list_type *object_list,
  ppr_template_id_list_type *object_ids,
  ppr_gallery_type *gallery);

static void write_reference_image(
  ppr_raw_image_type *raw_image_a,
  ppr_object_list_type object_list_a,
  ppr_template_id_list_type *object_ids_a,
  sdk_utils_id_color_list_type *color_list);

static void write_query_image(
  const char *query_image_name,
  ppr_context_type context,
  ppr_raw_image_type *raw_image_b,
  ppr_similarity_matrix_type similarity_matrix,
  ppr_object_list_type object_list_b,
  ppr_template_id_list_type *object_ids_b,
  sdk_utils_id_color_list_type *color_list);

/*****************************************************************************
 * Functions
 *****************************************************************************/

int main(int argc, char *argv[])
{
  int image_num;                           /* Index for looping over images                           */

  parameters_struct params;                /* Parameters from command-line                            */
  int i_first_input_file;                  /* Index of first file name on command line                */
  int i_input_file;                        /* Index for looping through input files                   */
  char extension[FILENAME_MAX];            /* Extension of current input file                         */
  int using_list;                          /* Flag set when using image list                          */
  ppr_context_type context;                /* SDK detection context                                   */

  char *reference_image_name;              /* Name of reference image                                 */
  char *query_file_name;                   /* Name of query file (image or list)                      */
  char *query_image_name;                  /* Name of current query image                             */
  sdk_utils_file_list_type image_list;     /* List of query images to process                         */
  int n_images;                            /* Number of query images to process                       */

  ppr_raw_image_type raw_image_a;          /* Raw reference image                                     */
  ppr_raw_image_type raw_image_b;          /* Raw query image                                         */

  ppr_object_list_type object_list_a;      /* Objects found in reference image                        */
  ppr_object_list_type object_list_b;      /* Objects found in query image                            */
  ppr_gallery_type reference_gallery;      /* Templates for objects in reference image                */
  ppr_gallery_type query_gallery;          /* Templates for objects in query image                    */

  sdk_utils_id_color_list_type color_list; /* Color assigned to each reference object ID              */
  ppr_template_id_list_type object_ids_a;  /* ID assigned to template for each reference image object */
  ppr_template_id_list_type object_ids_b;  /* ID assigned to template for each query image object     */
  ppr_similarity_matrix_type sim_matrix;   /* Score from comparing query gallery to reference gallery */

  /****************************/
  /* Configure and Initialize */
  /****************************/

  sdk_profile_timers_init();
  sdk_profile_timers_name_timer(0, "total");
  sdk_profile_timers_name_timer(1, "initialization");
  sdk_profile_timers_name_timer(2, "read image");
  sdk_profile_timers_name_timer(3, "create galleries");
  sdk_profile_timers_name_timer(4, "compare galleries");
  sdk_profile_timers_name_timer(5, "write overlay");
  sdk_profile_timers_start_timer(0);

  i_first_input_file = parse_command_line(argc, argv, &params);
  reference_image_name = argv[i_first_input_file];

  context = ppr_get_context();
  TRY(ppr_enable_recognition(context));

  TRY(ppr_set_license(context, my_license_id, my_license_key));
  TRY(ppr_set_models_path(context, params.models_path));
  TRY(ppr_set_detection_precision(context, params.detection_precision));
  TRY(ppr_set_landmark_detector_type(context, params.landmark_detector_type, PPR_AUTOMATIC_LANDMARKS));
  TRY(ppr_set_num_detection_threads(context, params.num_threads));
  TRY(ppr_set_search_pruning_aggressiveness(context, params.search_pruning));
  TRY(ppr_set_num_recognition_threads(context, params.num_threads));
  TRY(ppr_set_min_size(context, params.min_size));
  TRY(ppr_set_max_size(context, params.max_size));
  TRY(ppr_set_adaptive_min_size(context, params.adaptive_min_size));
  TRY(ppr_set_adaptive_max_size(context, params.adaptive_max_size));
  TRY(ppr_set_detection_threshold(context, params.detection_threshold));
  TRY(ppr_set_frontal_yaw_constraint(context, params.frontal_yaw_constraint));

  sdk_utils_print_context_configuration(stdout, context);

  printf("Initializing...\n");
  
  sdk_profile_timers_start_timer(1);
  TRY(ppr_initialize_context(context));
  sdk_profile_timers_stop_timer(1);

  /***************************/
  /* Process Reference Image */
  /***************************/

  printf("Reading reference image: '%s' ...\n", reference_image_name);

  /* Read reference image */
  sdk_profile_timers_start_timer(2);
  TRY_RAW_IMAGE(ppr_raw_image_io_read(reference_image_name, &raw_image_a));
  sdk_profile_timers_stop_timer(2);

  /* Create gallery containing objects in reference image */
  sdk_profile_timers_start_timer(3);
  create_gallery_from_image(context, &raw_image_a, &object_list_a, &object_ids_a, &reference_gallery);
  sdk_profile_timers_stop_timer(3);

  printf("Writing reference image with overlays...\n");

  /* Allocate list to hold color of each object in reference image */
  sdk_utils_id_color_list_allocate(&color_list, object_list_a.num_objects);

  /* Write reference image, returning color for each id */
  sdk_profile_timers_start_timer(5);
  write_reference_image(&raw_image_a, object_list_a, &object_ids_a, &color_list);
  sdk_profile_timers_stop_timer(5);

  /* Print objects found in reference image */
  printf("\n");
  printf("**********************************************************************\n");
  printf("REFERENCE IMAGE: %s\n", reference_image_name);
  printf("**********************************************************************\n");
  sdk_utils_print_object_list(stdout, object_list_a, RESULTS_INDENT, 0); 

  /********************************/
  /* Read and Submit Query Images */
  /********************************/

  for (i_input_file = i_first_input_file + 1 ; i_input_file < argc; i_input_file++)
  {
    query_file_name = argv[i_input_file];

    /* Determine if input file is image or image list */
    sdk_utils_get_file_extension(query_file_name, extension);
    if (strcmp(extension, "txt") == 0)
      using_list = 1;
    else
      using_list = 0;

    if (using_list)
    {
      sdk_utils_read_file_list(query_file_name, &image_list);
      n_images = image_list.size;
    }
    else
    {
      n_images = 1;
    }

    for (image_num = 0; image_num < n_images; image_num++)
    {
      if (using_list)
        query_image_name = image_list.filenames[image_num];
      else
        query_image_name = query_file_name;

      printf("\n");
      printf("Reading query image: '%s' ...\n", query_image_name);

      /* Read query image */
      sdk_profile_timers_start_timer(2);
      TRY_RAW_IMAGE(ppr_raw_image_io_read(query_image_name, &raw_image_b));
      sdk_profile_timers_stop_timer(2);

      /* Create gallery containing objects in query image */
      sdk_profile_timers_start_timer(3);
      create_gallery_from_image(context, &raw_image_b, &object_list_b, &object_ids_b, &query_gallery);
      sdk_profile_timers_stop_timer(3);

      /*******************/
      /* Compare Results */
      /*******************/

      printf("\n");
      printf("**********************************************************************\n");
      printf("QUERY IMAGE:      %s\n", query_image_name);
      printf("**********************************************************************\n");
      sdk_utils_print_object_list(stdout, object_list_b, RESULTS_INDENT, 0); 

      printf("\n");
      printf("Comparing objects in Query Image to objects in Reference Image\n");

      sdk_profile_timers_start_timer(4);
      TRY(ppr_compare_galleries(context, query_gallery, reference_gallery, &sim_matrix));
      sdk_profile_timers_stop_timer(4);

      printf("Writing overlay image...\n");

      /* Write query image */
      sdk_profile_timers_start_timer(5);
      write_query_image(query_image_name, context, &raw_image_b, sim_matrix, object_list_b, &object_ids_b, &color_list);
      sdk_profile_timers_stop_timer(5);

      /* Free memory allocated for current query image */
      ppr_free_gallery(query_gallery);
      ppr_free_similarity_matrix(sim_matrix);

      ppr_raw_image_free(raw_image_b);
      ppr_free_object_list(object_list_b);
      free(object_ids_b.template_ids);
    }

    if (using_list)
      sdk_utils_free_file_list(image_list);
  }

  /************/
  /* Finalize */
  /************/

  sdk_utils_id_color_list_free(color_list);
  ppr_free_gallery(reference_gallery);

  ppr_raw_image_free(raw_image_a);
  ppr_free_object_list(object_list_a);
  free(object_ids_a.template_ids);

  ppr_release_context(context);
  ppr_finalize_sdk();

  printf("Processing complete\n");

  sdk_profile_timers_stop_timer(0);
  sdk_profile_timers_print_timers();

  return EXIT_SUCCESS;
}

/*******************************************************************************
 * Local Functions
 ******************************************************************************/

/*
 * Create a gallery with templates from all objects found in input image
 */
static void create_gallery_from_image(
  ppr_context_type context,
  ppr_raw_image_type *raw_image,
  ppr_object_list_type *object_list,
  ppr_template_id_list_type *object_ids,
  ppr_gallery_type *gallery)
{
  int i;                                   /* Loop index                                    */

  ppr_image_type image;                    /* Image submitted to detector                   */
  ppr_object_suitability_type suitability; /* Value indicating if template can be extracted */
  ppr_template_type curr_template;         /* Template for current object                   */
  int id;                                  /* Template ID                                   */

  TRY(ppr_create_image(*raw_image, &image));

  /* Detect faces in image */
  printf("Detecting faces in image...\n");
  TRY(ppr_detect_objects(context, image, object_list));

  /* Generate templates for each object in reference image */
  printf("Generating templates in image...\n");
  TRY(ppr_create_gallery(context, gallery));

  object_ids->num_template_ids = object_list->num_objects;
  object_ids->template_ids = (int*)malloc(object_ids->num_template_ids * sizeof(int));
  sdk_utils_check_allocation(object_ids->template_ids, "object_ids->template_ids", __FUNCTION__);

  for (i = 0; i < object_list->num_objects; i++)
  {
    TRY(ppr_is_object_suitable_for_recognition(context, object_list->objects[i], &suitability));

    if (suitability == PPR_OBJECT_SUITABLE_FOR_RECOGNITION)
    {
      TRY(ppr_extract_template_from_object(context, image, object_list->objects[i], &curr_template));
      TRY(ppr_copy_template_to_gallery(context, gallery, curr_template, &id));

      object_ids->template_ids[i] = id;
      ppr_free_template(curr_template);
    }
    else
    {
      object_ids->template_ids[i] = -1;
    }
  }

  ppr_free_image(image);
}

/*****************************************************************************/

/*
 * Write reference image with different colors for different frontal faces
 */
static void write_reference_image(
  ppr_raw_image_type *raw_image_a,
  ppr_object_list_type object_list_a,
  ppr_template_id_list_type *object_ids_a,
  sdk_utils_id_color_list_type *color_list)
{
  int i;                                               /* Loop index                       */

  int color;                                           /* Color assigned to current object */
  sdk_draw_utils_line_attributes_type line_attributes; /* Attributes for drawing overlays  */

  sdk_draw_utils_init_line_attributes(&line_attributes);
  
  /* Write reference image with each frontal face a different color */
  color = 0;
  for (i = 0; i < object_list_a.num_objects; i++)
  {
    if (object_ids_a->template_ids[i] == -1)
    {
      if (object_list_a.objects[i].confidence > MIN_CONFIDENCE_THRESHOLD)
      {
        line_attributes.color = UNKNOWN_OBJECT_COLOR;
        line_attributes.line_type = SDK_DRAW_UTILS_DOTTED_LINE;
        line_attributes.thickness = UNKNOWN_OBJECT_THICKNESS;
        sdk_utils_overlay_object(raw_image_a, object_list_a.objects[i], "", line_attributes);
      }
    }
    else
    {
      sdk_utils_id_color_list_append(color_list, object_ids_a->template_ids[i], color);

      line_attributes.color = color;
      line_attributes.line_type = SDK_DRAW_UTILS_SOLID_LINE;
      line_attributes.thickness = HIGH_CONFIDENCE_THICKNESS;
      
      sdk_utils_overlay_object(raw_image_a, object_list_a.objects[i], "", line_attributes);
      color = (color + 1) % SDK_DRAW_UTILS_NUM_COLORS;
      if (color == UNKNOWN_OBJECT_COLOR)
        color = (color + 1) % SDK_DRAW_UTILS_NUM_COLORS;
    }
  }

  TRY_RAW_IMAGE(ppr_raw_image_io_write(REFERENCE_OVERLAY_IMAGE_NAME, *raw_image_a));
}

/*****************************************************************************/

/*
 * Write output images showing matches between query image and reference image
 */
static void write_query_image(
  const char *query_image_name,
  ppr_context_type context,
  ppr_raw_image_type *raw_image_b,
  ppr_similarity_matrix_type sim_matrix,
  ppr_object_list_type object_list_b,
  ppr_template_id_list_type *object_ids_b,
  sdk_utils_id_color_list_type *color_list)
{
  int i;                                               /* ID index                                                  */
  int j;                                               /* Object index                                              */

  ppr_template_id_list_type query_id_list;             /* List of IDs for all objects that produce templates        */
  ppr_template_id_list_type best_reference_ids;        /* ID of best matching template for each template in ID list */
  ppr_score_list_type score_list;                      /* Scores corresponding to each match                        */

  int thickness;                                       /* Line thickness for overlays                               */
  sdk_draw_utils_color_type color;                     /* Color for overlays                                        */

  char title[FILENAME_MAX];                            /* Name of input image without extension and directory       */
  char out_filename[FILENAME_MAX];                     /* Name of output image                                      */

  sdk_draw_utils_line_attributes_type line_attributes; /* Attributes for drawing overlays                           */

  sdk_draw_utils_init_line_attributes(&line_attributes);

  TRY(ppr_get_query_template_id_list_from_similarity_matrix(context, sim_matrix, &query_id_list));

  TRY(ppr_get_best_matching_templates(context, sim_matrix, query_id_list, QUERIES_ARE_UNIQUE, &best_reference_ids,
                                      &score_list));

  i = 0;
  for (j = 0; j < object_list_b.num_objects; j++)
  {
    if (object_ids_b->template_ids[j] == -1)
    {
      if (object_list_b.objects[j].confidence > MIN_CONFIDENCE_THRESHOLD)
      {
        line_attributes.color = UNKNOWN_OBJECT_COLOR;
        line_attributes.line_type = SDK_DRAW_UTILS_DOTTED_LINE;
        line_attributes.thickness = UNKNOWN_OBJECT_THICKNESS;
        sdk_utils_overlay_object(raw_image_b, object_list_b.objects[j], "", line_attributes);
      }
    }
    else
    {
      if (score_list.scores[i] > MEDIUM_CONFIDENCE_THRESHOLD)
      {
        color = sdk_utils_id_color_list_get_color(*color_list, best_reference_ids.template_ids[i]);
        if (score_list.scores[i] > HIGH_CONFIDENCE_THRESHOLD)
          thickness = HIGH_CONFIDENCE_THICKNESS;
        else
          thickness = MEDIUM_CONFIDENCE_THICKNESS;
        
        line_attributes.color = color;
        line_attributes.line_type = SDK_DRAW_UTILS_SOLID_LINE;
        line_attributes.thickness = thickness;
        sdk_utils_overlay_object(raw_image_b, object_list_b.objects[j], "", line_attributes);
      }
      else
      {
        line_attributes.color = UNKNOWN_OBJECT_COLOR;
        line_attributes.line_type = SDK_DRAW_UTILS_DOTTED_LINE;
        line_attributes.thickness = UNKNOWN_OBJECT_THICKNESS;
        sdk_utils_overlay_object(raw_image_b, object_list_b.objects[j], "", line_attributes);
      }
      i++;
    }
  }
  
  sdk_utils_get_file_title(query_image_name, title);
  sprintf(out_filename, "%s_query.%s", title, QUERY_IMAGE_FORMAT);

  TRY_RAW_IMAGE(ppr_raw_image_io_write(out_filename, *raw_image_b));

  ppr_free_template_id_list(query_id_list);
  ppr_free_template_id_list(best_reference_ids);
  ppr_free_score_list(score_list);
}

/******************************************************************************/
