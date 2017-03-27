#ifndef _FILE_TO_IMAGE_H_
#define _FILE_TO_IMAGE_H_

#define PICTURE_HIGHT      60
#define PICTURE_WIDTH      80

#define pix_white 254.0
#define pix_black 0.0
extern int pixdata[PICTURE_WIDTH*PICTURE_HIGHT];
extern char buffer[PICTURE_HIGHT*PICTURE_WIDTH / 4];

void file_to_image(char * path);
#endif