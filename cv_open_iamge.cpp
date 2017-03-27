#include <iostream>
#include <cv.h>
#include <highgui.h>
#include <cxcore.h>
#include "file_to_image.h"

void cv_open_image(char * path)
{
	IplImage *img = cvLoadImage(path, 1);
	CvScalar s;
	//read data
	for (int i = 0; i < img->height; i++)
	{
		for (int j = 0; j < img->width; j++)
		{
			s = cvGet2D(img, i, j); // get the (i,j) pixel value
			pixdata[i*PICTURE_WIDTH + j] = (int)s.val[0];
			printf("%.1f %.1f %.1f ", s.val[0], s.val[1], s.val[2]);
			cvSet2D(img, i, j, s);//set the (i,j) pixel value
		}
		printf("\n");
	}
}