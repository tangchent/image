/*****************************************
�� �� ����ImageProcess.c
��    ����ͼ����
�޸����ڣ�2017-3-16
*****************************************/
#include <stdio.h>
#include "ImageProcess.h"
#include "file_to_image.h"

#define CONSOLE_PRINT 1
//һ�������߶�Ӧͼ��Ŀ�����СΪ10
#define BLOCK_COUNT_MIN 9
//���18
#define BLOCK_COUNT_MAX 18
//ʮ������������6����������
#define LINE_CONFIRM_MIN 5

#define LEFT_START 2
#define RIGHT_END  PICTURE_WIDTH-3

#define LEFT_BREAK	2
#define RIGHT_BREAK PICTURE_WIDTH-3
unsigned char lastFirstMidPoint = 38;//��һ���е�
unsigned char middlePoint = 38;
//ͼ������
//extern unsigned char Image[];
int * Image = pixdata;

//unsigned char end = 0;
unsigned char EndPosition = 0;//��ǰ�����ı߽�������
unsigned char MidLineCompleteFlag = 0;//������ȡ��ɱ�־
signed char MidPoints[PICTURE_HIGHT] = { 0 };//����ÿ���е�
signed char Row;//��
signed char Col;//��
unsigned char stopFlag = 0;
signed char boundLeft[PICTURE_HIGHT] = { 0 };
signed char boundRight[PICTURE_HIGHT] = { 0 };
extern unsigned char MidSpeed;
extern unsigned char LowSpeed;

//POINT ����ͼ������������Ӧ�ĺ�������
//0 1 .... PICTURE_WIDTH-1
//   ---------------->X
//0 |
//1 |
//. |
//. |
//. |
//. ��
//  Y
//X ��[0,PICTURE_WIDTH-1],Y��[0,PICTURE_HIGHT-1]
POINT leftBoundPoints[PICTURE_HIGHT];
POINT rightBoundPoints[PICTURE_HIGHT];

unsigned char cross = 0;
unsigned char LowLeft = 0;
unsigned char LowRight = 0;
unsigned char Left = 0;
unsigned char Right = 0;


//������е�ʶ��
//��ɫ������10cm����2.5cm
//��55��ɨ����40��
unsigned char isStopLine()
{
	char startLine;
	char endLine;
	unsigned char i, j;
	unsigned char lineConfirmCnt;
	unsigned char blockCount, color;
	unsigned char blockLength;

	lineConfirmCnt = 0;
	startLine = PICTURE_HIGHT - 5;
	endLine = PICTURE_HIGHT - 20;
	for (i = startLine; i > endLine; --i)
	{
		color = *(Image + i*PICTURE_WIDTH + 0);
		blockCount = 0;
		for (j = 0; j < PICTURE_WIDTH - 3; ++j)
		{
			//������ͬ��ɫΪһ��
			if (*(Image + i*PICTURE_WIDTH + j) == color &&
				*(Image + i*PICTURE_WIDTH + j + 1) == color &&
				*(Image + i*PICTURE_WIDTH + j + 2) == color)
			{
				j += 3;
				blockLength = 3;//�鳤��
				while (*(Image + i*PICTURE_WIDTH + j) == color && (j<PICTURE_WIDTH - 1))
				{
					blockLength++;
					++j;
				}
				color = *(Image + i*PICTURE_WIDTH + j);
				//�����������߶�Ӧ�����3-5֮��
				if (blockLength > 2 && blockLength < 6)
				{
					blockCount++;
				}
			}
		}
		//һ����������
		if (blockCount > BLOCK_COUNT_MIN && blockCount < BLOCK_COUNT_MAX)
		{
			lineConfirmCnt++;
			if (lineConfirmCnt > LINE_CONFIRM_MIN)
			{
				lineConfirmCnt = 1;
				return lineConfirmCnt;
			}
		}
	}

	lineConfirmCnt = 0;
	return lineConfirmCnt;
}

void CenterLineExtraction(void)
{
	static unsigned char isStop = 0;
	signed char i, j;
	unsigned char color;
	unsigned char searchPosition;
	unsigned char leftFind = 0;
	unsigned char rightFind = 0;
	POINT leftSerchPoint;
	POINT rightSerchPoint;
	unsigned char leftBoundCnt;
	unsigned char rightBoundCnt;
	unsigned char isHorizontal;
	signed char dxLeft;
	signed char dxRight;

	if (isStopLine())
	{
		isStop++;
#if CONSOLE_PRINT
		printf("\nSTOP LINE\n");
#endif
		/*PTD14_O = 1;
		DELAY_MS(50);
		PTD14_O = 0;*/
	}
	else
		isStop = 0;
	if (isStop)
	{
		stopFlag = 1;
		for (i = 0; i < PICTURE_HIGHT - 2; ++i)
		{
			MidPoints[i] = middlePoint;
		}
		return;
	}
#if CONSOLE_PRINT
	printf("LAST MID:%d\n",lastFirstMidPoint);
#endif
	i = PICTURE_HIGHT - 2;
	//�����һ�ε������е��Ǻ�ɫ��˵��ʲô�ط����˴�
	//��ʱӦ��ɨ��ȫ��
	//���ڴ�������ұ�ɨ�軹���ұ������ɨ��Ӧ�ø�����һ��ͼ���ƫ������
	//�����һ������ת�Ľ�������ʱͼ��ı߽����������ߣ���ʱӦ�ô�������ұ�ɨ��
	//������ұ������ɨ��

	if (*(Image + i*PICTURE_WIDTH + lastFirstMidPoint) == BLACK_POINT
		&& *(Image + (i - 1)*PICTURE_WIDTH + lastFirstMidPoint) == BLACK_POINT
		)
	{
		while (!leftFind&&!rightFind&&i > 5)
		{
			for (j = 0; j < PICTURE_WIDTH - 3; ++j)
			{
				//��ɫ���ɫ����߽�
				if (
					(*(Image + i*PICTURE_WIDTH + j - 2) == BLACK_POINT) &&
					(*(Image + i*PICTURE_WIDTH + j - 1) == BLACK_POINT) &&
					(*(Image + i*PICTURE_WIDTH + j) == BLACK_POINT) &&
					(*(Image + i*PICTURE_WIDTH + j + 1) == WHITE_POINT) &&
					(*(Image + i*PICTURE_WIDTH + j + 2) == WHITE_POINT)
					)
				{
					*(boundLeft + i) = j;
					leftFind = 1;
					break;
				}
				//��ɫ���ɫ���ұ߽�
				if
					(
					(*(Image + i*PICTURE_WIDTH + j) == WHITE_POINT) &&
					(*(Image + i*PICTURE_WIDTH + j - 1) == WHITE_POINT) &&
					(*(Image + i*PICTURE_WIDTH + j - 2) == WHITE_POINT) &&
					(*(Image + i*PICTURE_WIDTH + j + 1) == BLACK_POINT) &&
					(*(Image + i*PICTURE_WIDTH + j + 2) == BLACK_POINT)
					)
				{
					*(boundRight + i) = j;
					rightFind = 1;
					break;
				}
			}
			if (leftFind && rightFind)
			{
				lastFirstMidPoint = (unsigned char)((*(boundLeft + i) + *(boundRight + i)) / 2);
				leftSerchPoint.x = *(boundLeft + i);
				leftSerchPoint.y = i;
				rightSerchPoint.x = *(boundRight + i);
				rightSerchPoint.y = i;
			}
			else if (!leftFind && rightFind)
			{
				*(boundLeft + i) = 0;
				lastFirstMidPoint = (unsigned char)((*(boundRight + i)) / 2);
				rightSerchPoint.x = *(boundRight + i);
				rightSerchPoint.y = i;
			}
			else if (leftFind && !rightFind)
			{
				*(boundRight + i) = PICTURE_WIDTH - 1;
				lastFirstMidPoint = (unsigned char)((*(boundLeft + i) + *(boundRight + i)) / 2);
				leftSerchPoint.x = *(boundLeft + i);
				leftSerchPoint.y = i;
			}
			else
			{
				*(boundLeft + i) = 2;
				*(boundRight + i) = PICTURE_WIDTH - 1;
				lastFirstMidPoint = middlePoint;
			}
			*(MidPoints + i) = lastFirstMidPoint;
#if CONSOLE_PRINT
			printf("LINE %d's MID POINT:%d ,LEFT:%d RIGHT:%d\n", i, lastFirstMidPoint, *(boundLeft + i), *(boundRight + i));
#endif
			--i;
		}
		
	}
	else
	{
		while (!leftFind&&!rightFind&&i>5)
		{
			leftFind = 0;
			rightFind = 0;
			searchPosition = lastFirstMidPoint;
			//������һ��ɨ������߽�
			for (j = 0; j < searchPosition; ++j)
			{
				//��ɫ���ɫ����߽�
				if (
					(*(Image + i*PICTURE_WIDTH + j) == BLACK_POINT) &&
					(*(Image + i*PICTURE_WIDTH + j - 1) == BLACK_POINT) &&
					(*(Image + i*PICTURE_WIDTH + j + 1) == WHITE_POINT) &&
					(*(Image + i*PICTURE_WIDTH + j + 2) == WHITE_POINT) &&
					(*(Image + i*PICTURE_WIDTH + j + 3) == WHITE_POINT)
					)
				{
					*(boundLeft + i) = j;
					leftFind = 1;
					break;
				}
			}
			searchPosition = PICTURE_WIDTH - 3;
			//������һ��ɨ�����ұ߽�
			for (; j < searchPosition; ++j)
			{
				//��ɫ���ɫ���ұ߽�
				if
					(
					(*(Image + i*PICTURE_WIDTH + j) == WHITE_POINT) &&
					(*(Image + i*PICTURE_WIDTH + j - 1) == WHITE_POINT) &&
					(*(Image + i*PICTURE_WIDTH + j - 2) == WHITE_POINT) &&
					(*(Image + i*PICTURE_WIDTH + j + 1) == BLACK_POINT) &&
					(*(Image + i*PICTURE_WIDTH + j + 2) == BLACK_POINT)
					)
				{
					*(boundRight + i) = j;
					rightFind = 1;
					break;
				}
			}

			if (leftFind && rightFind)
			{
				lastFirstMidPoint = (unsigned char)((*(boundLeft + i) + *(boundRight + i)) / 2);
				leftSerchPoint.x = *(boundLeft + i);
				leftSerchPoint.y = i;
				rightSerchPoint.x = *(boundRight + i);
				rightSerchPoint.y = i;
			}
			else if (!leftFind && rightFind)
			{
				*(boundLeft + i) = 0;
				lastFirstMidPoint = (unsigned char)((*(boundRight + i)) / 2);
				rightSerchPoint.x = *(boundRight + i);
				rightSerchPoint.y = i;
			}
			else if (leftFind && !rightFind)
			{
				*(boundRight + i) = PICTURE_WIDTH - 1;
				lastFirstMidPoint = (unsigned char)((*(boundLeft + i) + *(boundRight + i)) / 2);
				leftSerchPoint.x = *(boundLeft + i);
				leftSerchPoint.y = i;
			}
			else
			{
				*(boundLeft + i) = 0;
				*(boundRight + i) = PICTURE_WIDTH - 1;
				lastFirstMidPoint = middlePoint;
			}
			*(MidPoints + i) = lastFirstMidPoint;
#if CONSOLE_PRINT
			printf("LINE %d's MID POINT:%d ,LEFT:%d RIGHT:%d\n",i, lastFirstMidPoint, *(boundLeft + i),*(boundRight + i));
#endif
			--i;
		}
	}

	leftBoundCnt = 0;
	rightBoundCnt = 0;
	leftBoundPoints->x = -1;
	rightBoundPoints->x = -1;
	isHorizontal = 1;//���е�ɨ����ˮƽ��
	while (i > 5)
	{
		//���ұ߽綼�ҵ���
		if (leftFind&&rightFind)
		{
			leftFind = 0;
			i = leftSerchPoint.y;
			j = leftSerchPoint.x - 6;//�˴����Ը���dx���޶���Χ
			if (j < 1) j = 1;
			searchPosition = leftSerchPoint.x + 6;
			if (searchPosition > PICTURE_WIDTH - 1) searchPosition = PICTURE_WIDTH - 1;
			for (; j < searchPosition; ++j)
			{
				color = *(Image + i*PICTURE_WIDTH + j);
				//ͻ��㣬��߽�
				if (
					(*(Image + i*PICTURE_WIDTH + j - 1) == color) &&
					(*(Image + i*PICTURE_WIDTH + j + 1) == !color)
					)
				{
					(leftBoundPoints + leftBoundCnt)->x = j;
					(leftBoundPoints + leftBoundCnt)->y = i;
					leftFind = 1;
					leftSerchPoint.x = j;
					leftSerchPoint.y = i - 1;
					leftBoundCnt++;
					break;
				}
			}
			rightFind = 0;
			i = rightSerchPoint.y;
			j = rightSerchPoint.x - 6;//�˴����Ը���dx���޶���Χ
			if (j < 1) j = 1;
			searchPosition = rightSerchPoint.x + 6;
			if (searchPosition > PICTURE_WIDTH - 1) searchPosition = PICTURE_WIDTH - 1;
			for (; j < searchPosition; ++j)
			{
				color = *(Image + i*PICTURE_WIDTH + j);
				//ͻ��㣬��߽�
				if (
					(*(Image + i*PICTURE_WIDTH + j - 1) == color) &&
					(*(Image + i*PICTURE_WIDTH + j + 1) == !color)
					)
				{
					(rightBoundPoints + rightBoundCnt)->x = j;
					(rightBoundPoints + rightBoundCnt)->y = i;
					rightFind = 1;
					rightSerchPoint.x = j;
					rightSerchPoint.y = i - 1;
					rightBoundCnt++;
					break;
				}
			}
			if (!leftFind&&!rightFind)
			{
				break;
			}
			if (leftBoundCnt > 1)
			{
				dxLeft = (leftBoundPoints + leftBoundCnt)->x - (leftBoundPoints + leftBoundCnt - 1)->x;
#if CONSOLE_PRINT
				printf("dx left at line %d is %d", (leftBoundPoints + leftBoundCnt)->y, dxLeft);
#endif
			}

		}
		//ֻ�ҵ����
		else if (leftFind)
		{
			leftFind = 0;
			i = leftSerchPoint.y;
			j = leftSerchPoint.x - 6;//�˴����Ը���dx���޶���Χ
			if (j < 0) j = 0;
			searchPosition = leftSerchPoint.x + 6;
			if (searchPosition > PICTURE_WIDTH - 1) searchPosition = PICTURE_WIDTH - 1;
			for (; j < searchPosition; ++j)
			{
				//ͻ��㣬��߽�
				if (
					(*(Image + i*PICTURE_WIDTH + j) == BLACK_POINT) &&
					(*(Image + i*PICTURE_WIDTH + j + 1) == WHITE_POINT)
					)
				{
					(leftBoundPoints + leftBoundCnt)->x = j;
					(leftBoundPoints + leftBoundCnt)->y = i;
					leftFind = 1;
					leftSerchPoint.x = j;
					leftSerchPoint.y = i - 1;
					leftBoundCnt++;
					break;
				}
			}
			if ((leftSerchPoint.x > RIGHT_BREAK || leftSerchPoint.x < LEFT_BREAK) && leftBoundCnt > 20)
			{
				break;
			}
			for (j = PICTURE_WIDTH - 1; j > leftSerchPoint.x + 5; --j)
			{
				if (
					(*(Image + i*PICTURE_WIDTH + j) == BLACK_POINT) &&
					(*(Image + i*PICTURE_WIDTH + j - 1) == WHITE_POINT)
					)
				{
					(rightBoundPoints + rightBoundCnt)->x = j;
					(rightBoundPoints + rightBoundCnt)->y = i;
					rightFind = 1;
					rightSerchPoint.x = j;
					rightSerchPoint.y = i - 1;
					rightBoundCnt++;
					break;
				}
			}
		}
		//ֻ�ҵ��ұ�
		else if (rightFind)
		{
			rightFind = 0;
			i = rightSerchPoint.y;
			j = rightSerchPoint.x - 6;//�˴����Ը���dx���޶���Χ
			if (j < 1) j = 1;
			searchPosition = rightSerchPoint.x + 6;
			if (searchPosition > PICTURE_WIDTH - 1) searchPosition = PICTURE_WIDTH - 1;
			for (; j < searchPosition; ++j)
			{
				color = *(Image + i*PICTURE_WIDTH + j);
				//ͻ��㣬��߽�
				if (
					(*(Image + i*PICTURE_WIDTH + j - 1) == color) &&
					(*(Image + i*PICTURE_WIDTH + j + 1) == !color)
					)
				{
					(rightBoundPoints + rightBoundCnt)->x = j;
					(rightBoundPoints + rightBoundCnt)->y = i;
					rightFind = 1;
					rightSerchPoint.x = j;
					rightSerchPoint.y = i - 1;
					rightBoundCnt++;
					break;
				}
			}
			if ((rightSerchPoint.x > RIGHT_BREAK || rightSerchPoint.x < LEFT_BREAK) && rightBoundCnt > 20)
			{
				break;
			}
			for (j = 0; j < rightSerchPoint.x - 5; ++j)
			{
				if (
					(*(Image + i*PICTURE_WIDTH + j) == BLACK_POINT) &&
					(*(Image + i*PICTURE_WIDTH + j + 1) == WHITE_POINT)
					)
				{
					(leftBoundPoints + leftBoundCnt)->x = j;
					(leftBoundPoints + leftBoundCnt)->y = i;
					leftFind = 1;
					leftSerchPoint.x = j;
					leftSerchPoint.y = i - 1;
					leftBoundCnt++;
					break;
				}
			}
		}
		else
		{
			//����Ӧ��
			//11111                  0000
			//111                   0000
			//0000                  0000
			//000                    000 ������� i > ������
			//000                  00000
			//0000                   011
			//000000                0111
			//000000               11111
			//000000              011111 
			if (leftBoundCnt == 0 && rightBoundCnt < 21)
			{
				i = rightSerchPoint.y;
				for (; i > rightSerchPoint.y - 20&&i>0; --i)
				{
					for (j = 0; j < rightSerchPoint.x - 5; ++j)
					{
						if (
							(*(Image + i*PICTURE_WIDTH + j) == BLACK_POINT) &&
							(*(Image + i*PICTURE_WIDTH + j + 1) == WHITE_POINT)
							)
						{
							(leftBoundPoints + leftBoundCnt)->x = j;
							(leftBoundPoints + leftBoundCnt)->y = i;
							leftFind = 1;
							leftSerchPoint.x = j;
							leftSerchPoint.y = i - 1;
							leftBoundCnt++;
							break;
						}
					}
				}
				if (!leftFind)
					break;
			}
			else if (rightBoundCnt == 0 && leftBoundCnt < 21)
			{
				i = leftSerchPoint.y;
				for (; i > rightSerchPoint.y - 20 && i>0; --i)
				{
					for (j = PICTURE_WIDTH - 1; j > leftSerchPoint.x + 5; --j)
					{
						if (
							(*(Image + i*PICTURE_WIDTH + j) == BLACK_POINT) &&
							(*(Image + i*PICTURE_WIDTH + j - 1) == WHITE_POINT)
							)
						{
							(rightBoundPoints + rightBoundCnt)->x = j;
							(rightBoundPoints + rightBoundCnt)->y = i;
							rightFind = 1;
							rightSerchPoint.x = j;
							rightSerchPoint.y = i - 1;
							rightBoundCnt++;
							break;
						}
					}
				}
				if (!rightFind)
					break;
			}
			//������ұ߽���������ǱȽϽ����ģ�Ҳ�ǳ����˴���ֻ��������ɨ��
			else if (leftSerchPoint.y > 52&&rightSerchPoint.y > 52)
			{
				i = leftSerchPoint.y;
				while (!leftFind&&!rightFind&&i>5)
				{
					leftFind = 0;
					rightFind = 0;
					searchPosition = lastFirstMidPoint;
					//������һ��ɨ������߽�
					for (j = 0; j < searchPosition; ++j)
					{
						//��ɫ���ɫ����߽�
						if (
							(*(Image + i*PICTURE_WIDTH + j) == BLACK_POINT) &&
							(*(Image + i*PICTURE_WIDTH + j - 1) == BLACK_POINT) &&
							(*(Image + i*PICTURE_WIDTH + j + 1) == WHITE_POINT) &&
							(*(Image + i*PICTURE_WIDTH + j + 2) == WHITE_POINT) &&
							(*(Image + i*PICTURE_WIDTH + j + 3) == WHITE_POINT)
							)
						{
							*(boundLeft + i) = j;
							leftFind = 1;
							break;
						}
					}
					searchPosition = PICTURE_WIDTH - 3;
					//������һ��ɨ�����ұ߽�
					for (; j < searchPosition; ++j)
					{
						//��ɫ���ɫ���ұ߽�
						if
							(
							(*(Image + i*PICTURE_WIDTH + j) == WHITE_POINT) &&
							(*(Image + i*PICTURE_WIDTH + j - 1) == WHITE_POINT) &&
							(*(Image + i*PICTURE_WIDTH + j - 2) == WHITE_POINT) &&
							(*(Image + i*PICTURE_WIDTH + j + 1) == BLACK_POINT) &&
							(*(Image + i*PICTURE_WIDTH + j + 2) == BLACK_POINT)
							)
						{
							*(boundRight + i) = j;
							rightFind = 1;
							break;
						}
					}

					if (leftFind && rightFind)
					{
						lastFirstMidPoint = (unsigned char)((*(boundLeft + i) + *(boundRight + i)) / 2);
						leftSerchPoint.x = *(boundLeft + i);
						leftSerchPoint.y = i;
						rightSerchPoint.x = *(boundRight + i);
						rightSerchPoint.y = i;
					}
					else if (!leftFind && rightFind)
					{
						*(boundLeft + i) = 0;
						lastFirstMidPoint = (unsigned char)((*(boundRight + i)) / 2);
						rightSerchPoint.x = *(boundRight + i);
						rightSerchPoint.y = i;
					}
					else if (leftFind && !rightFind)
					{
						*(boundRight + i) = PICTURE_WIDTH - 1;
						lastFirstMidPoint = (unsigned char)((*(boundLeft + i) + *(boundRight + i)) / 2);
						leftSerchPoint.x = *(boundLeft + i);
						leftSerchPoint.y = i;
					}
					else
					{
						*(boundLeft + i) = 0;
						*(boundRight + i) = PICTURE_WIDTH - 1;
						lastFirstMidPoint = middlePoint;
					}
					*(MidPoints + i) = lastFirstMidPoint;
#if CONSOLE_PRINT
					printf("LINE %d's MID POINT:%d ,LEFT:%d RIGHT:%d\n", i, lastFirstMidPoint, *(boundLeft + i), *(boundRight + i));
#endif
					--i;
				}
			}
		}
		--i;
	}
	(leftBoundPoints + leftBoundCnt)->x = -1;
	(rightBoundPoints + rightBoundCnt)->x = -1;
}