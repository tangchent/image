#include "file_to_image.h"
#include <iostream>
#include <fstream> 
using namespace std;

int pixdata[PICTURE_WIDTH*PICTURE_HIGHT];
char buffer[PICTURE_HIGHT*PICTURE_WIDTH / 4];

unsigned char data_res(unsigned char c)
{
	unsigned char d[4];
	d[0] = c & 0x08;
	d[1] = c & 0x04;
	d[2] = c & 0x02;
	d[3] = c & 0x01;
	return d[0] + (d[1] << 1) + (d[2] << 2) + (d[3] << 3);
}
int char_get(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	else if (c == 'A')
		return 10;
	else if (c == 'B')
		return 11;
	else if (c == 'C')
		return 12;
	else if (c == 'D')
		return 13;
	else if (c == 'E')
		return 14;
	else
		return 15;
}
void image_get(char * buf)
{
	int * image_address = pixdata;
	unsigned char data  = 0;
	unsigned char data1 = 0;
	unsigned char data2 = 0;
	int i;
	for (i = 0; i <= (PICTURE_HIGHT*PICTURE_WIDTH / 4) - 1;){
		data1 = char_get(*(buf + i++));
		data2 = char_get(*(buf + i++));
		data = data2 + (data1 << 4);
		/**image_address++ = data & 0x01;
		*image_address++ = (data >> 1) & 0x01;
		*image_address++ = (data >> 2) & 0x01;
		*image_address++ = (data >> 3) & 0x01;
		*image_address++ = (data >> 4) & 0x01;
		*image_address++ = (data >> 5) & 0x01;*/
		
		*image_address++ = (data >> 7)& 0x01;
		*image_address++ = (data >> 6) & 0x01;
		*image_address++ = (data >> 5) & 0x01;
		*image_address++ = (data >> 4) & 0x01;
		*image_address++ = (data >> 3) & 0x01;
		*image_address++ = (data >> 2) & 0x01;
		*image_address++ = (data >> 1) & 0x01;
		*image_address++ = data & 0x01;
	}
}

void file_to_image(char * path)
{
	ifstream in_file(path);
	if (!in_file.is_open())
	{
		cout << "Error opening file"<<endl;
		return;
	}

	char c;
	int cnt = 0;
	while ((c = in_file.get()) != EOF){
		*(buffer + cnt) = c;
		++cnt;
		cout << c;
	}
	cout << endl;
	in_file.close();
	image_get(buffer);
}