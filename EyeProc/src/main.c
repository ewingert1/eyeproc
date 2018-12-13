#include "processing.h"
#include "stdlib.h"
#include <stdio.h>
#include <errno.h>

extern void process_image_asm(uint32_t width, uint32_t height, uint8_t* ImgSource, uint8_t* Img1, uint8_t* Img2, uint8_t* ImgFinale);

int main(int* argc, char** argv){
	Image_t *Entree=createImage(),
					*Gray=createImage(),
					*Edges=createImage(),
					*InvEdges=createImage(),
					*Pad32=createImage();



	loadRAW("lena256x256.raw",256,256,3,Entree);
	allocate(Pad32,Entree->height,Entree->width,RGB32);
	pad24to32RGB(Entree,Pad32);
	saveRAW(Pad32,"LenaPad32.raw");
	allocate(Gray,Pad32->height,Pad32->width,UNSIGNED8);
	rgb2gray(Pad32,Gray);
	saveRAW(Gray,"LenaGray.raw");
	process_image_asm(Entree->width,Entree->height,Pad32->data_ptr,Gray->data_ptr,Edges->data_ptr,InvEdges->data_ptr);

	// clone(Gray,Edges);
	// edgedetection(Gray,Edges,SOBEL);
	// clone(Edges,InvEdges);
	// saveRAW(Edges,"LenaSOBEL.raw");
	// invert(Edges,InvEdges);
	// saveRAW(InvEdges,"LenaSOBELinv.raw");
	//
	// edgedetection(Gray,Edges,PREWITT);
	// saveRAW(Edges,"LenaPREWITT.raw");
	// invert(Edges,InvEdges);
	// saveRAW(InvEdges,"LenaPREWITTinv.raw");

	deleteImage(Entree);
	deleteImage(Gray);
	deleteImage(Edges);
	deleteImage(InvEdges);
	deleteImage(Pad32);

	return 0;
}
