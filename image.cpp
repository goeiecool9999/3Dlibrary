#include "image.hpp"
#include <iostream>
#include <fstream>
#include <jpeglib.h>
#include <png.h>
#include <string>

Image loadImageFromJpeg(std::string fileName) {
	jpeg_decompress_struct decompress;
	jpeg_error_mgr jerr;
	decompress.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&decompress);
	FILE* file = NULL;
	if((file = fopen(fileName.c_str(), "rb")) == NULL) {
		std::cout << "File doesnt exist or could not be opened." << std::endl;
		Image img;
		img.data = NULL;
		img.width = 0;
		img.height = 0;
		return img;
	}
	jpeg_stdio_src(&decompress, file);
	jpeg_read_header(&decompress, true);
	decompress.out_color_space = JCS_RGB;
	jpeg_start_decompress(&decompress);
	unsigned stride = decompress.image_width*decompress.num_components;
	unsigned height = decompress.image_height;
	JSAMPARRAY data = new JSAMPROW[height];
	for(int i = 0; i < height; i++) {
		data[i] = new JSAMPLE[stride];
	}
	JSAMPARRAY temp = new JSAMPROW;
	*temp = new JSAMPLE[stride];
	int row = 0;
	while (decompress.output_scanline < height) {	
		jpeg_read_scanlines(&decompress, temp, 1);
		for(int i = 0; i < stride; i++){
			data[row][i] = (*temp)[i];
		}
		row++;
	}
	delete[] *temp;
	delete temp;
	Image img;
	img.data = data;
	img.width = decompress.image_width;
	img.height = height;
	img.bytesperpix = 3;
	return img;
}

Image loadImageFromPng(std::string fileName) {
	
	FILE *fp = fopen(fileName.c_str(), "rb");
	Image img;
	img.data = NULL;
	img.width = 0;
	img.height = 0;
	img.bytesperpix = 0;
	if (!fp)
	{
		return img;
	}
	png_bytep header = new unsigned char[8];
	fread(header, 1, 8, fp);
	bool isPng = !png_sig_cmp(header, 0, 8);
	if (!isPng)
	{
		std::cout << "It's not PNG :(" << std::endl;
		return img;
	}
	std::cout << "It's PNG!" << std::endl;
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp)NULL, NULL, NULL);
	if(!png_ptr) {
		return img;
	}
	
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		png_destroy_read_struct(&png_ptr,
		  (png_infopp)NULL, (png_infopp)NULL);
		return img;
	}
	png_infop end_info = png_create_info_struct(png_ptr);
	if (!end_info)
	{
		png_destroy_read_struct(&png_ptr, &info_ptr,
			(png_infopp)NULL);
		return img;
	}
	
	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);
	png_set_keep_unknown_chunks(png_ptr, PNG_HANDLE_CHUNK_NEVER, NULL, 9);
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_ALPHA, NULL);
	img.width = png_get_image_width(png_ptr, info_ptr);
	img.height = png_get_image_height(png_ptr, info_ptr);
	img.data = new unsigned char*[img.height];
	for(int i = 0;i < img.height; i++)
		img.data[i] = new unsigned char[img.width*3];
	png_bytepp data = png_get_rows(png_ptr, info_ptr);
	for(int y = 0; y < img.height; y++) {
		for(int x = 0; x < img.width*3; x++) {
			img.data[y][x] = data[y][x];
		}
	}
	img.bytesperpix = 3;
	return img;
}
