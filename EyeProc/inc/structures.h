#ifndef STRUCTURES_H_INCLUDED
#define STRUCTURES_H_INCLUDED

#include <stdint.h>
#include <stdlib.h>

typedef enum{UNSIGNED8=1,UNSIGNED16=2,RGB24=3,RGB32=4} PixelFormat_t;
typedef enum{PREWITT=1,SOBEL=2,ROBERTS=3} EdgeDetectionMethod_t;

typedef struct Image {
	uint8_t* data_ptr;
	uint32_t height;
	uint32_t width;
	PixelFormat_t pixelformat;
	uint8_t isAllocated;
} Image_t;

typedef struct Histogramme{
	uint32_t size; //redondance
	uint32_t *data_ptr;
} Histogramme_t;

typedef int Mask_t[3][3];

#endif
