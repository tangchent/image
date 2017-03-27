#include <iostream>
#include <cv.h>
#include <highgui.h>
#include <cxcore.h>
#include <string.h>
#include "file_to_image.h"
#include "cv_open_image.h"
#include "perspective_transformation.h"
#include "ImageProcess.h"

using namespace std;

extern POINT leftBoundPoints[];
extern int pixdata[];
extern char buffer[];

IplImage* src = 0;//�Ŵ��ͼ��
IplImage* dst = 0;//���ڱ���Ŵ��ͼ����ʾ����ʱʹ��
IplImage *perspective_transformation_img;//��͸�Ӻ��ͼ��
IplImage *cvImg;//��txt��ȡ����ͼ��
char filename[] = ".\\one\\00001.TXT";
int file_count = 0;

void convert(void)
{
	int count = file_count;
	int cnt = 4;
	char buf[6];
	buf[0] = '0';
	buf[1] = '0';
	buf[2] = '0';
	buf[3] = '0';
	buf[4] = '0';
	buf[5] = '\0';
	while (count > 0)
	{
		*(buf + cnt) = (count % 10) + '0';
		count /= 10;
		cnt--;
	}
	cnt = 4;
	int len = strlen(filename);
	for (int i = len-5; cnt >=0; --i,--cnt)
	{
		*(filename + i) = *(buf + cnt);
	}
}
void show_image(char * filename)
{
	mem_set();
	cout << filename << endl;
	file_to_image(filename);//��txt��������ݶ�ȡ��pixdata����

	CvSize imgSize;
	CvScalar s;
	int i, j;
	imgSize.width = PICTURE_WIDTH + 20;
	imgSize.height = PICTURE_HIGHT + 20;
	cvImg = cvCreateImage(imgSize, 8, 3);
	imgSize.width = PICTURE_WIDTH + 200;
	imgSize.height = PICTURE_HIGHT + 200;
	perspective_transformation_img = cvCreateImage(imgSize, 8, 3);
	perspective_transformation(perspective_transformation_img);//����pixdata������͸�ӱ任

	
	for (i = 0; i < PICTURE_HIGHT; i++)//i����y��
		//for (i = PICTURE_HIGHT - 1; i>=0; i--)//i����y��
	{
		for (j = 0; j<PICTURE_WIDTH; j++)//j����x��
		{
			int x, y;
			float tmp;
			if (pixdata[i*PICTURE_WIDTH + j] == 0)
				tmp = pix_white;
			else
				tmp = pix_black;
			s.val[0] = tmp;
			s.val[1] = tmp;
			s.val[2] = tmp;
			y = i;
			x = j;
			//x = (int)((21.2132 - (float)i*30.0) / (5.36396 + 0.707106*(float)i))+37;
			//y = (int)((21.2132*j) / (20.2132 + 0.707106*(float)i));
			cvSet2D(cvImg, y, x, s);
		}
	}
	imgSize.width = cvImg->width * 8.0;
	imgSize.height = cvImg->height * 8.0;

	//����ͼ������  
	src = cvCreateImage(imgSize, cvImg->depth, cvImg->nChannels);
	cvResize(cvImg, src, CV_INTER_AREA);
	dst = cvCloneImage(src);
	cvCopy(dst, src, 0);

	pix_fill();
	get_perspective_transformation_img();
	cvShowImage("ShowBigImage", src);
	cvShowImage("ShowSrcImage", cvImg);
	cvShowImage("ShowPTImage", perspective_transformation_image);
	cvMoveWindow("ShowPTImage",10,10);
	cvMoveWindow("ShowSrcImage",10, 200);
	cvSaveImage(".//a.bmp", cvImg);
	cvSaveImage(".//aa.bmp", perspective_transformation_image);
	CenterLineExtraction();
}

void on_mouse(int event, int x, int y, int flags, void* ustc)
{
	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5, 0, 1, CV_AA);
	CvFont font1;
	cvInitFont(&font1, CV_FONT_HERSHEY_SIMPLEX, 1, 1, 0, 1, CV_AA);
	if (event == CV_EVENT_MOUSEMOVE)
	{
		cvCopy(dst, src, 0);
		CvPoint pt = cvPoint(x, y);
		CvPoint pt1 = cvPoint(700, 100);
		CvPoint pt2 = cvPoint(650, 200);

		char temp[16];
		sprintf(temp, "(%d,%d)", pt.x/8, pt.y/8);
		
		cvPutText(src, temp, pt, &font, cvScalar(34, 177, 76, 0));
		cvPutText(src, "NEXT", pt1, &font1, cvScalar(34, 177, 76, 0));
		cvPutText(src, "PREVIOUS", pt2, &font1, cvScalar(34, 177, 76, 0));
		cvCircle(src, pt, 2, cvScalar(255, 0, 0, 0), CV_FILLED, CV_AA, 0);
		cvShowImage("ShowBigImage", src);
	}
	else if (event == CV_EVENT_LBUTTONDOWN)
	{
		CvPoint pt = cvPoint(x, y);
		printf("%d %d \n", x, y);
		if (x >= 700 && x <= 780 && y >= 80 && y <= 100)//next
		{
			file_count++;
			convert();
			show_image(filename);
		}
		else if (x >= 650 && x <= 800 && y >= 180 && y <= 200)//pervious
		{
			if (file_count != 0)
				file_count--;
			convert();
			show_image(filename);
		}
	}
}

int main(int argc, char** argv)
{
	cvNamedWindow("ShowBigImage");
	cvNamedWindow("ShowSrcImage");
	cvNamedWindow("ShowPTImage");
	cvSetMouseCallback("ShowBigImage", on_mouse, 0);
	show_image(filename);
	while (1)
	{
		int key = cvWaitKey(0);
		printf("******%d****\n", key);
		if (key == 27)
			break;
		else if (key == 97)
		{
			if (file_count != 0)
				file_count--;
			convert();
			show_image(filename);
		}
		else if (key == 100)
		{
			file_count++;
			convert();
			show_image(filename);
		}
	}
	cvDestroyWindow("ShowBigImage");
	cvReleaseImage(&src);
	cvDestroyWindow("ShowSrcImage");
	cvReleaseImage(&cvImg);
	cvDestroyWindow("ShowPTImage");
	cvReleaseImage(&perspective_transformation_image);
	return 0;
}