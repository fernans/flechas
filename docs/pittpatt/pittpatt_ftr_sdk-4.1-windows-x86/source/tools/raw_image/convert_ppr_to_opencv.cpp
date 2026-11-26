#include <stdlib.h>
#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include "pittpatt_raw_image_io.h"

IplImage *convert_ppr_image_to_opencv(ppr_raw_image_type *p_image)
{
  IplImage *cv_image;
  unsigned char *data;
  int i,j,k;

  ppr_raw_image_convert(p_image, PPR_RAW_IMAGE_BGR24);
  cv_image = cvCreateImage(cvSize(p_image->width, p_image->height), IPL_DEPTH_8U, 3);

  data = (unsigned char*)cv_image->imageData;

  for (i=0; i<p_image->height; i++)
    for (j=0; j<p_image->width; j++)
      for (k=0; k<3; k++)
        data[i * cv_image->widthStep + j * 3 + k] =
          p_image->data[i * p_image->bytes_per_line + j * 3 + k];

  return cv_image;
}

ppr_raw_image_type convert_opencv_image_to_ppr(IplImage *cv_image)
{
  ppr_raw_image_type p_image;
  unsigned char *data;
  int i,j,k;

  ppr_raw_image_create(&p_image, cv_image->width, cv_image->height, PPR_RAW_IMAGE_BGR24);

  data = (unsigned char*)cv_image->imageData;

  for (i=0; i<p_image.height; i++)
    for (j=0; j<p_image.width; j++)
      for (k=0; k<3; k++)
        p_image.data[i * p_image.bytes_per_line + j * 3 + k] = 
          data[i * cv_image->widthStep + j * 3 + k];

  return p_image;
}


int main(int argc, char *argv[])
{
  ppr_raw_image_type ppr_image;
  ppr_raw_image_error_type err;
  IplImage *opencv_image; 

  if(argc<2)
  {
    printf("Usage: main <image-file-name>\n\7");
    exit(0);
  }

  err = ppr_raw_image_io_read(argv[1], &ppr_image);

  if(err != PPR_RAW_IMAGE_SUCCESS)
  {
    printf("Could not load image file: %s\n",argv[1]);
    exit(0);
  }

  opencv_image = convert_ppr_image_to_opencv(&ppr_image);

  cvNamedWindow("mainWin", CV_WINDOW_AUTOSIZE); 
  cvMoveWindow("mainWin", 100, 100);
  cvShowImage("mainWin", opencv_image);
  cvWaitKey(0);

  /* uncomment to test opencv -> ppr conversion */
  /*
  ppr_raw_image_free(ppr_image);
  ppr_image = convert_opencv_image_to_ppr(opencv_image);
  err = ppr_raw_image_io_write("output.png", ppr_image);
  */

  ppr_raw_image_free(ppr_image);
  cvReleaseImage(&opencv_image);
  return 0;
}
