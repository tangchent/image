#include <iostream>
#include <cv.h>
#include <highgui.h>
#include <cxcore.h>
#include "file_to_image.h"

#define IMAGE_HIGHT_ADD 30
#define IMAGE_WIDTH_ADD 30
using namespace cv;

IplImage * perspective_transformation_image;
int perspective_transformation_pixdata[(PICTURE_HIGHT + IMAGE_HIGHT_ADD) * (PICTURE_WIDTH + IMAGE_WIDTH_ADD)] = { 0 };

void mem_set(void)
{
	memset((void*)perspective_transformation_pixdata, -1, sizeof(perspective_transformation_pixdata));
}

void perspective_transformation(IplImage * p)
{
	int i, j;
	CvScalar ss;

	Point2f src[4]; //源图像中的点  
	Point2f dst[4]; //目标图像中的点  

	src[0].x = 24;
	src[0].y = 25;
	src[1].x = 53;
	src[1].y = 27;
	src[2].x = 19;
	src[2].y = 51;
	src[3].x = 55;
	src[3].y = 49;

	dst[0].x = 15;
	dst[0].y = 20;
	dst[1].x = 60;
	dst[1].y = 22;
	dst[2].x = 14;
	dst[2].y = 53;
	dst[3].x = 61;
	dst[3].y = 50;

	Mat t;  //变换矩阵  
	t = getPerspectiveTransform(src, dst);

	printf("transform matrix\n");
	for (int i = 0; i<3; i++)
	{
		printf("% .4f ", t.at<double>(0, i));
		printf("% .4f ", t.at<double>(1, i));
		printf("% .4f \n", t.at<double>(2, i));
	}

	for (i = 55; i >= 20; i--)//i代表y轴
	{
		for (j = 15; j<70; j++)//j代表x轴
		{
			int x, y;
			float tmp;
			if (pixdata[i*PICTURE_WIDTH + j] == 0)
				tmp = pix_white;
			else
				tmp = pix_black;
			ss.val[0] = tmp;
			ss.val[1] = tmp;
			ss.val[2] = tmp;
			Mat sample = (Mat_<double>(3, 1) << j, i, 1); //样例点 (400,240)  
			Mat r = t*sample;  //变换矩阵乘以样例点，得到结果点的其次坐标  

			double s = r.at<double>(2, 0);  //归一化尺度  

			y = (int)ceil(r.at<double>(0, 0) / s);
			x = (int)ceil(r.at<double>(1, 0) / s);

			if (tmp - 0.0 > 1e-6)
				perspective_transformation_pixdata[x *(PICTURE_WIDTH + IMAGE_WIDTH_ADD) + y] = (int)pix_white;
			else
				perspective_transformation_pixdata[x *(PICTURE_WIDTH + IMAGE_WIDTH_ADD) + y] = (int)pix_black;
			//x = (int)((21.2132 - (float)i*30.0) / (5.36396 + 0.707106*(float)i))+37;
			//y = (int)((21.2132*j) / (20.2132 + 0.707106*(float)i));
			cvSet2D(p, x, y, ss);
		}
	}
	printf("\nresult...\n");
	cvSaveImage(".//aa.bmp", p);
}

IplImage * get_perspective_transformation_img(void)
{
	CvSize imgSize;
	CvScalar s;
	int i, j;

	imgSize.width = PICTURE_WIDTH + IMAGE_WIDTH_ADD;
	imgSize.height = PICTURE_HIGHT + IMAGE_HIGHT_ADD;
	perspective_transformation_image = cvCreateImage(imgSize, 8, 3);

	for (i = 0; i < PICTURE_HIGHT + IMAGE_HIGHT_ADD; i++)//i代表y轴
	//for (i = PICTURE_HIGHT + 199; i >= 0; i--)//i代表y轴
	{
		for (j = 0; j<PICTURE_WIDTH + IMAGE_WIDTH_ADD; j++)//j代表x轴
		{
			int x, y;
			float tmp;
			if (perspective_transformation_pixdata[i*(PICTURE_WIDTH + IMAGE_WIDTH_ADD) + j] == 0)
				tmp = pix_black;
			else if (perspective_transformation_pixdata[i*(PICTURE_WIDTH + IMAGE_WIDTH_ADD) + j] == 254)
				tmp = pix_white;
			else
				tmp = 100.0;
			s.val[0] = tmp;
			s.val[1] = tmp;
			s.val[2] = tmp;
			x = j;
			y = i;
			//x = (int)((21.2132 - (float)i*30.0) / (5.36396 + 0.707106*(float)i))+37;
			//y = (int)((21.2132*j) / (20.2132 + 0.707106*(float)i));
			cvSet2D(perspective_transformation_image, y, x, s);
		}
	}
	return perspective_transformation_image;
}

void pix_fill(void)
{
	int sum;
	int i, j;
	i = 0;
	if ((perspective_transformation_pixdata[0] == -1))
	{
		if ((perspective_transformation_pixdata[1] == pix_black))
		{
			perspective_transformation_pixdata[0] = pix_black;
		}
		else if (perspective_transformation_pixdata[1] == pix_white)
		{
			perspective_transformation_pixdata[0] = pix_white;
		}
		else if (perspective_transformation_pixdata[PICTURE_WIDTH + IMAGE_WIDTH_ADD + 1] == pix_white)
		{
			perspective_transformation_pixdata[0] = pix_white;
		}
	}
	for (j = 1; j < PICTURE_WIDTH + IMAGE_WIDTH_ADD; ++j)
	{
		if (perspective_transformation_pixdata[j] == -1)
		{
			if (perspective_transformation_pixdata[j - 1] == pix_black)
			{
				perspective_transformation_pixdata[0] = pix_black;
			}
			else if (perspective_transformation_pixdata[j - 1] == pix_white)
			{
				perspective_transformation_pixdata[0] = pix_white;
			}
			else if (perspective_transformation_pixdata[PICTURE_WIDTH + IMAGE_WIDTH_ADD + j - 1] == pix_white)
			{
				perspective_transformation_pixdata[0] = pix_white;
			}
			else 
				perspective_transformation_pixdata[0] = pix_white;
		}
	}
	//
	for (i = 5; i < PICTURE_HIGHT + IMAGE_HIGHT_ADD - 5; ++i)
	{
		for (j = 5; j < PICTURE_WIDTH + IMAGE_WIDTH_ADD - 5; ++j)
		{
			if (perspective_transformation_pixdata[i*(PICTURE_WIDTH + IMAGE_WIDTH_ADD) + j] == -1)
			{
				sum = perspective_transformation_pixdata[i*(PICTURE_WIDTH + IMAGE_WIDTH_ADD) + j - 1] +
					perspective_transformation_pixdata[i*(PICTURE_WIDTH + IMAGE_WIDTH_ADD) + j + 1] +
					perspective_transformation_pixdata[(i - 1)*(PICTURE_WIDTH + IMAGE_WIDTH_ADD) + j] +
					perspective_transformation_pixdata[(i - 1)*(PICTURE_WIDTH + IMAGE_WIDTH_ADD) + j - 1] +
					perspective_transformation_pixdata[(i - 1)*(PICTURE_WIDTH + IMAGE_WIDTH_ADD) + j + 1] +
					perspective_transformation_pixdata[(i + 1)*(PICTURE_WIDTH + IMAGE_WIDTH_ADD) + j] +
					perspective_transformation_pixdata[(i + 1)*(PICTURE_WIDTH + IMAGE_WIDTH_ADD) + j - 1] +
					perspective_transformation_pixdata[(i + 1)*(PICTURE_WIDTH + IMAGE_WIDTH_ADD) + j + 1];
				if (sum > (254 * 3))
					perspective_transformation_pixdata[i*(PICTURE_WIDTH + IMAGE_WIDTH_ADD) + j] = pix_white;
				else 
					perspective_transformation_pixdata[i*(PICTURE_WIDTH + IMAGE_WIDTH_ADD) + j] = pix_black;
				if (perspective_transformation_pixdata[i*(PICTURE_WIDTH + IMAGE_WIDTH_ADD) + j - 1] == pix_white&&perspective_transformation_pixdata[i*(PICTURE_WIDTH + IMAGE_WIDTH_ADD) + j + 1] == pix_white)
				{
					perspective_transformation_pixdata[i*(PICTURE_WIDTH + IMAGE_WIDTH_ADD) + j] = pix_white;
				}
				else if (perspective_transformation_pixdata[i*(PICTURE_WIDTH + IMAGE_WIDTH_ADD) + j - 1] == pix_black&&perspective_transformation_pixdata[i*(PICTURE_WIDTH + IMAGE_WIDTH_ADD) + j + 1] == pix_black)
				{
					perspective_transformation_pixdata[i*(PICTURE_WIDTH + IMAGE_WIDTH_ADD) + j] = pix_black;
				}
			}
		}
	}
}