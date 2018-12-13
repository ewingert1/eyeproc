#ifndef PROCESSING_H_INCLUDED
#define PROCESSING_H_INCLUDED

#include <stdint.h>
#include "structures.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void loadRAW(char* filename, uint32_t Height , uint32_t Width, PixelFormat_t Format, Image_t* result);
void saveRAW(Image_t* input, char* filename);
void allocate(Image_t* frame, uint32_t height, uint32_t width, PixelFormat_t Format);
void deallocate(Image_t* frames);
void clone(Image_t* input, Image_t* output);
void rgb2gray(Image_t* input, Image_t* output);
void histogram(Image_t* frame,Histogramme_t* hist);
void edgedetection(Image_t* input, Image_t* output, EdgeDetectionMethod_t method);
void invert(Image_t* input, Image_t* output);
Image_t* createImage();
void deleteImage(Image_t* image);
void pad24to32RGB(Image_t* Image, Image_t* dest);
void vectorize(Image_t* Image, Image_t* dest);
uint32_t getPixelValue(Image_t*, uint32_t, uint32_t);
void setPixelValue(Image_t*, uint32_t, uint32_t, uint32_t);


#endif
