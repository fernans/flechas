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
 * TOOL : Add Landmarks to Objects
 *
 * Given an object list file this program adds landmarks to all objects within
 * it.  If an object already has landmarks, it is skipped.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pittpatt_ftr_sdk.h"
#include "pittpatt_license.h"
#include "pittpatt_raw_image_io.h"
#include "sdk_utils.h"
#include "models_path.h"

/*******************************************************************************
 * Constants
 ******************************************************************************/

#define NUM_DETECTION_THREADS sdk_utils_get_num_cpus()

/*******************************************************************************
 * Functions
 ******************************************************************************/

int main(int argc, char *argv[])
{
  int i;
  ppr_error_type err;
  ppr_context_type context;
  ppr_landmark_detector_type landmark_detector;
  ppr_object_list_type object_list;
  char source_image_name[PPR_MAX_SOURCE_FILE_NAME_SIZE];
  ppr_raw_image_type raw_image;
  ppr_image_type image;
  
  if (argc != 3)
  {
    fprintf(stderr, "usage: %s <object list> <landmark detector>\n", argv[0]);
    fprintf(stderr, "  where landmark detector is:\n");
    fprintf(stderr, "    frontal\n");
    fprintf(stderr, "    multi_pose\n");
    fprintf(stderr, "    dual_frontal\n");
    fprintf(stderr, "    dual_multi_pose\n");
    return EXIT_FAILURE;
  }
  
  context = ppr_get_context();

  TRY(ppr_set_license(context, my_license_id, my_license_key));
  TRY(ppr_set_models_path(context, MODELS_PATH));
  TRY(ppr_set_num_detection_threads(context, NUM_DETECTION_THREADS));

  if (strcmp(argv[2], "frontal") == 0)
  {
    landmark_detector = PPR_FRONTAL_LANDMARK_DETECTOR;
  }
  else if (strcmp(argv[2], "multi_pose") == 0)
  {
    landmark_detector = PPR_MULTI_POSE_LANDMARK_DETECTOR;
  }
  else if (strcmp(argv[2], "dual_frontal") == 0)
  {
    landmark_detector = PPR_DUAL_FRONTAL_LANDMARK_DETECTOR;
  }
  else if (strcmp(argv[2], "dual_multi_pose") == 0)
  {
    landmark_detector = PPR_DUAL_MULTI_POSE_LANDMARK_DETECTOR;
  }
  else
  {
    fprintf(stderr, "Invalid landmark detector type: %s\n", argv[2]);
    fprintf(stderr, "  Valid options are:\n");
    fprintf(stderr, "    frontal\n");
    fprintf(stderr, "    multi_pose\n");
    fprintf(stderr, "    dual_frontal\n");
    fprintf(stderr, "    dual_multi_pose\n");
    return EXIT_FAILURE;
  }

  TRY(ppr_set_landmark_detector_type(context, landmark_detector, PPR_MANUAL_LANDMARKS));

  printf("Initializing...\n");
  TRY(ppr_initialize_context(context));  
  
  printf("Reading object list %s...\n", argv[1]);  
  TRY(ppr_read_object_list(context, argv[1], &object_list, source_image_name));

  printf("Reading source image %s...\n", source_image_name);
  TRY_RAW_IMAGE(ppr_raw_image_io_read(source_image_name, &raw_image));
  TRY(ppr_create_image(raw_image, &image));

  for (i = 0; i < object_list.num_objects; ++i)
  {
    printf("Detecting landmarks in object[%d] of %d...\n", i, object_list.num_objects);
    err = ppr_detect_landmarks_from_object(context, image, &object_list.objects[i]);
    if (err == PPR_OBJECT_ALREADY_CONTAINS_LANDMARKS)
    {
      printf("Object already has landmarks...skipping\n");
    }
    else
    {
      sdk_utils_check_ppr_error(err, "ppr_detect_landmarks_from_object");
    }
  }

  printf("Writing object list %s...\n", argv[1]);
  TRY(ppr_write_object_list(context, argv[1], object_list, source_image_name));

  ppr_free_image(image);
  ppr_raw_image_free(raw_image);
  ppr_free_object_list(object_list);

  ppr_release_context(context);
  ppr_finalize_sdk();
  
  return EXIT_SUCCESS;
}

/******************************************************************************/
