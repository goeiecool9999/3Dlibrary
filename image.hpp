#ifndef IMAGE_H
#define IMAGE_H

#include <string>

typedef unsigned char** ImageData;
struct Image {
	ImageData data;
	unsigned int width,height,bytesperpix;
};

unsigned long getColorFromPixel(int x, int y, Image img);

Image loadImageFromJpeg(std::string fileName);
Image loadImageFromPng(std::string fileName);

#endif
