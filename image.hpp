#ifndef IMAGE_H
#define IMAGE_H

#include <string>

typedef unsigned char** ImageData;
struct Image {
	ImageData data;
	unsigned int width,height,bytesperpix;
};

Image loadImageFromJpeg(std::string fileName);
Image loadImageFromPng(std::string fileName);

#endif
