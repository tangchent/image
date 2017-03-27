//dark foreast
//2017.03.17
//author TC
#ifndef _IMAGEPROCESS_H_
#define _IMAGEPROCESS_H_

#define BLACK_POINT 1
#define WHITE_POINT 0

typedef struct{
	signed short int x;
	signed short int y;
}POINT;
unsigned char isObstacle(void);
unsigned char isCircularRing(void);
unsigned char isStopLine(void);
unsigned char isOutOfBoundary(void);
void CenterLineExtraction(void);

#endif