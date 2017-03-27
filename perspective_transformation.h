#ifndef _PF_HHHH_
#define _PF_HHHH_

extern int perspective_transformation_pixdata[];
extern IplImage *perspective_transformation_image;

void mem_set(void);
void pix_fill(void);
void perspective_transformation(IplImage *);
IplImage * get_perspective_transformation_img(void);
#endif