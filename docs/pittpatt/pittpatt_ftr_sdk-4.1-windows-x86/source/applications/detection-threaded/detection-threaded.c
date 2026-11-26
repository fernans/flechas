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
 * EXAMPLE PROGRAM : Detection (Threaded)
 *
 * This is an example of threading the detector at the image level 
 * to detect faces in an image list.
 *
 * Use the -help flag for usage information.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "pittpatt_ftr_sdk.h"
#include "pittpatt_license.h"

#include "pittpatt_raw_image_io.h"

#include "parse_command_line.h"
#include "sdk_utils.h"

/*****************************************************************************
 * Constants
 *****************************************************************************/

#define MAX_NUM_THREADS             32
#define RESULTS_INDENT              0
#define VERBOSE_RESULTS             0
#define COLORS_BASED_ON_CONFIDENCE  1
#define OUTPUT_IMAGE_FORMAT         "jpg"

/*****************************************************************************
 * Data Types
 *****************************************************************************/

typedef struct {
  int thread_id;
  int best_frontal;
  int print;
  int overlay;
  int thumbnails;
  int binary_output;
} thread_data;

/*****************************************************************************
 * Global Data
 *****************************************************************************/

thread_data thread_data_array[MAX_NUM_THREADS];
int image_num = 0;
int n_images;
sdk_utils_file_list_type image_list;
pthread_mutex_t image_num_mutex;
pthread_mutex_t print_mutex;
ppr_context_type context;

/*****************************************************************************
 * Functions
 *****************************************************************************/

void* do_detection(void *td)
{
  thread_data *data = td;
  int my_image_num;
  char *image_name;
  int object_num;

  int best_frontal = data->best_frontal;
  int print = data->print;
  int overlay = data->overlay;
  int thumbnails = data->thumbnails;
  int binary_output = data->binary_output;

  char title[FILENAME_MAX];
  char out_filename[FILENAME_MAX];

  ppr_raw_image_type raw_image;
  ppr_image_type image;
  ppr_object_list_type object_list;
  ppr_raw_image_type thumbnail;

  while(1)
  {
    pthread_mutex_lock(&image_num_mutex);
    my_image_num = image_num;
    image_num++;
    pthread_mutex_unlock(&image_num_mutex);

    if (my_image_num >= n_images)
      break;

    image_name = image_list.filenames[my_image_num];
    sdk_utils_get_file_title(image_name, title);

    /* Read input image */
    TRY_RAW_IMAGE(ppr_raw_image_io_read(image_name, &raw_image));

    /* Convert raw image to pittpatt image type */
    TRY(ppr_create_image(raw_image, &image));
    
    /* Submit image to detector */
    if (best_frontal)
    {
      TRY(ppr_detect_best_frontal_object(context, image, &object_list));
    }
    else
    {
      TRY(ppr_detect_objects(context, image, &object_list));
    }
    
    /* Print the data from the objects to the screen */
    pthread_mutex_lock(&print_mutex);
    printf("\nThread %d processed image %s\n", data->thread_id, image_name);
    if (print)
    {
      sdk_utils_print_object_list(stdout, object_list, RESULTS_INDENT, VERBOSE_RESULTS);
    }
    pthread_mutex_unlock(&print_mutex);
    
    /* Overlay object boundaries and landmarks onto image and write to file */
    if (overlay)
    {
      sdk_utils_overlay_object_list(&raw_image, object_list, COLORS_BASED_ON_CONFIDENCE);
      sprintf(out_filename, "%s_overlay.%s", title, OUTPUT_IMAGE_FORMAT);
      TRY_RAW_IMAGE(ppr_raw_image_io_write(out_filename, raw_image));
    }

    /* Crop thumbnail of detected objects and write to file */
    if (thumbnails)
    {
      for (object_num = 0; object_num < object_list.num_objects; object_num++)
      {
        TRY(ppr_crop_thumbnail_from_image(context, image, object_list.objects[object_num], &thumbnail));
        sprintf(out_filename, "%s_thumbnail_%03d.%s", title, object_num, OUTPUT_IMAGE_FORMAT);
        TRY_RAW_IMAGE(ppr_raw_image_io_write(out_filename, thumbnail));
        ppr_raw_image_free(thumbnail);
      }
    }

    /* Write binary output file */
    if (binary_output)
    {
      if (best_frontal)
        sprintf(out_filename, "%s_best_frontal_object_list.bin", title);
      else
        sprintf(out_filename, "%s_object_list.bin", title);
      TRY(ppr_write_object_list(context, out_filename, object_list, image_name));
    }

    /* Free all objects allocated */
    ppr_raw_image_free(raw_image);
    ppr_free_image(image);
    ppr_free_object_list(object_list);
  }
  
  /* Leave the thread */
  ppr_finalize_thread();
  pthread_exit((void*)data);
}

/******************************************************************************/

int main(int argc, char *argv[])
{
  pthread_t thread[MAX_NUM_THREADS];
  pthread_attr_t attr;
  long t;

  parameters_struct params;
  int i_first_file;
  char extension[FILENAME_MAX];
  char *input_file_name;

  /****************************/
  /* Configure and Initialize */
  /****************************/

  i_first_file = parse_command_line(argc, argv, &params);

  if (params.num_threads > MAX_NUM_THREADS)
  {
    fprintf(stderr, "Invalid number of threads\n");
    exit(EXIT_FAILURE);
  }

  context = ppr_get_context();

  TRY(ppr_set_license(context, my_license_id, my_license_key));
  TRY(ppr_set_models_path(context, params.models_path));
  TRY(ppr_set_detection_precision(context, params.detection_precision));

  if (params.landmark_detector_type != PPR_NO_LANDMARK_DETECTOR)
    TRY(ppr_set_landmark_detector_type(context, params.landmark_detector_type, PPR_AUTOMATIC_LANDMARKS));

  TRY(ppr_set_search_pruning_aggressiveness(context, params.search_pruning));
  TRY(ppr_set_min_size(context, params.min_size));
  TRY(ppr_set_max_size(context, params.max_size));
  TRY(ppr_set_adaptive_min_size(context, params.adaptive_min_size));
  TRY(ppr_set_adaptive_max_size(context, params.adaptive_max_size));
  TRY(ppr_set_detection_threshold(context, params.detection_threshold));
  TRY(ppr_set_search_region(context, params.x1, params.y1, params.x2, params.y2));
  
  sdk_utils_print_context_configuration(stdout, context);

  printf("Initializing with %d threads...\n", params.num_threads);
  
  TRY(ppr_initialize_context(context));

  /**************************/
  /* Read and Submit Images */
  /**************************/

  input_file_name = argv[i_first_file];

  /* Determine if input file is an image list */
  sdk_utils_get_file_extension(input_file_name, extension);
  if (strcmp(extension, "txt") != 0)
  {
    fprintf(stderr, "File list required\n");
    exit(EXIT_FAILURE);
  }

  /* Read image list */
  sdk_utils_read_file_list(input_file_name, &image_list);
  n_images = image_list.size;

  /*************/
  /* Threading */
  /*************/

  /* Initialize mutex and attributes */
  pthread_mutex_init(&image_num_mutex, NULL);
  pthread_mutex_init(&print_mutex, NULL);
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  /* Create threads */
  for (t = 0; t < params.num_threads; t++)
  {
    int error_code;
    thread_data_array[t].thread_id = t;
    thread_data_array[t].best_frontal = params.best_frontal;
    thread_data_array[t].print = params.print;
    thread_data_array[t].overlay = params.overlay;
    thread_data_array[t].thumbnails = params.thumbnails;
    thread_data_array[t].binary_output = params.binary_output;

    error_code = pthread_create(&thread[t], &attr, do_detection, (void*) &thread_data_array[t]);
    if (error_code)
    {
      fprintf(stderr, "error creating pthread (err: %d)\n", error_code);
      exit(EXIT_FAILURE);
    }
  }

  /* Wait for threads to finish */
  pthread_attr_destroy(&attr);
  for (t = 0; t < params.num_threads; t++)
  {
    int error_code;
    thread_data *finished_thread;

    error_code = pthread_join(thread[t], (void**)&finished_thread);
    if (error_code)
    {
      fprintf(stderr, "error joining pthread (err: %d)\n", error_code);
      exit(EXIT_FAILURE);
    }
  }

  /************/
  /* Finalize */
  /************/

  /* Free image list */
  sdk_utils_free_file_list(image_list);

  /* Release the detection context since it is no longer needed */
  TRY(ppr_release_context(context));

  /* Finalize the SDK */
  ppr_finalize_sdk();

  printf("Processing complete\n");

  pthread_mutex_destroy(&image_num_mutex);
  pthread_mutex_destroy(&print_mutex);

  return EXIT_SUCCESS;
}

/*****************************************************************************/
