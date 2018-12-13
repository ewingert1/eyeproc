#include "processing.h"


#define Red(pixel) 	((pixel&0xFF0000)>>16)
#define Green(pixel) 	((pixel&0x00FF00)>>8)
#define Blue(pixel) 	((pixel&0x0000FF))


Mask_t SobelMaskX = {{-1,0,1},{-2,0,2},{-1,0,1}};
Mask_t SobelMaskY = {{-1,-2,-1},{0,0,0},{1,2,1}};
Mask_t PrewittMaskX = {{-1,0,1},{-1,0,1},{-1,0,1}};
Mask_t PrewittMaskY = {{-1,-1,-1},{0,0,0},{1,1,1}};


uint32_t getPixelValue(Image_t* ImgPtr, uint32_t i, uint32_t j){
	uint32_t pixel=0,offset;
	for(offset=0;offset<ImgPtr->pixelformat;++offset)
		pixel |= ((ImgPtr->data_ptr[ImgPtr->pixelformat*(i*ImgPtr->width+j)+offset])<<(8*offset));
	return pixel;
}

void setPixelValue(Image_t* ImgPtr, uint32_t i, uint32_t j, uint32_t value){
	uint32_t pixel = value , offset;
	for(offset=0;offset<ImgPtr->pixelformat;++offset){
		ImgPtr->data_ptr[ImgPtr->pixelformat*(i*ImgPtr->width+j)+offset] = pixel&0xFF;
		pixel>>=8;
	}
}

void loadRAW(char* filename, uint32_t Height , uint32_t Width, PixelFormat_t Format, Image_t* result){
	FILE *imageFile = fopen(filename,"rb");
	if(!imageFile)	printf("Cannot open file \n");
	allocate(result,Height,Width,Format);
	fread(result->data_ptr,Format*Width*Height,1,imageFile);
	fclose(imageFile);
}

void saveRAW(Image_t* input, char* filename){
	FILE *imageFile = fopen(filename,"w");
	uint32_t Height = input->height;
	uint32_t Width = input->width;
	PixelFormat_t Format = input->pixelformat;
	fwrite(input->data_ptr,Format*Width*Height,1,imageFile);
	fclose(imageFile);
}

Image_t* createImage(){
	Image_t* image = malloc(sizeof(Image_t));
	image->height = 0;
	image->width = 0;
	image->pixelformat = 0;
	image->isAllocated = 0;
	image->data_ptr = NULL;
}

void deleteImage(Image_t* image){
	deallocate(image);
	image->height = 0;
	image->width = 0;
	image->pixelformat = 0;
	image->isAllocated = 0;
	free(image);
}

void allocate(Image_t* frame, uint32_t Height, uint32_t Width, PixelFormat_t Format){
	frame->height = Height;
	frame->width = Width;
	frame->pixelformat = Format;
	frame->data_ptr = (uint8_t*)malloc(Height*Width*Format*sizeof(uint8_t));
	frame->isAllocated = 1;
}

void deallocate(Image_t* frame){
	free(frame->data_ptr);
}

void clone(Image_t* input, Image_t* output){
	allocate(output,input->height,input->width,input->pixelformat);
}

void rgb2gray(Image_t* input, Image_t* output){
	uint32_t i=0,j=0;
	for(i=0;i<input->height;++i){
		for(j=0;j<input->width;++j){
			uint32_t pixel = getPixelValue(input,i,j);//((uint32_t)input->data_ptr[i][input->pixelformat*j+2]<<16)|((uint32_t)input->data_ptr[i][input->pixelformat*j+1]<<8)|((uint32_t)input->data_ptr[i][input->pixelformat*j]);
			setPixelValue(output,i,j,(76*(uint32_t)Red(pixel)  + 150*(uint32_t)Green(pixel) + 29*(uint32_t)Blue(pixel))/(uint8_t)255);
		}
	}
}


void histogram(Image_t* frame, Histogramme_t* hist){
	uint32_t i,j;
	if(!hist || !frame) return;
	uint32_t maxPixelValue = ((1<<(8*frame->pixelformat))-1);
	hist->data_ptr = (uint32_t*)malloc( maxPixelValue*sizeof(uint32_t)); //malloc of (2^(8*bytesperpixel) - 1) double words (32 bits) for the histogram
	hist->size=maxPixelValue;
	for(i=0;i<maxPixelValue;i++)
		hist->data_ptr[i]=0;
	for(i=0;i<frame->height;++i)
		for(j=0;j<frame->width;++j)
			hist->data_ptr[/*frame->data_ptr[i][j]*/getPixelValue(frame,i,j)]++;
}


void edgedetection(Image_t* input, Image_t* output, EdgeDetectionMethod_t method){
	int Ix,Iy;
	int i,j,u,v;
	Mask_t *Mx,*My;

	if(method==SOBEL){
		Mx=&SobelMaskX;
		My=&SobelMaskY;
	} else if(method==PREWITT){
		Mx=&PrewittMaskX;
		My=&PrewittMaskY;
	}

	for(i=1;i<input->height-1;++i)
		for(j=1;j<input->width-1;++j){
			Ix=0,Iy=0;
			for(u=-1;u<=1;++u)
				for(v=-1;v<=1;++v){
					Ix+=(*Mx)[u+1][v+1]*getPixelValue(input,i+u,j+v); //input->data_ptr[i+u][j+v];
					Iy+=(*My)[u+1][v+1]*getPixelValue(input,i+u,j+v); //input->data_ptr[i+u][j+v];
				}
			/*output->data_ptr[i][j]*/setPixelValue(output,i,j,(uint8_t)sqrt(Ix*Ix+Iy*Iy));
		}
}

void invert(Image_t* input, Image_t* output){
	uint32_t i,j,maxPixelValue=( (1<<(8*input->pixelformat)) -1 );
	for(i=0;i<input->height;++i)
		for(j=0;j<input->width;++j)
			/*output->data_ptr[i][j]*/setPixelValue(output,i,j,maxPixelValue - getPixelValue(input,i,j));// input->data_ptr[i][j];
}

void pad24to32RGB(Image_t* Image, Image_t* dest){
	uint32_t i,j,width=Image->width;
	for (i=0; i<Image->height; ++i)
		for (j=0; j<Image->width; ++j){
			dest->data_ptr[4*(i*width+j)] 		= Image->data_ptr[3*(i*width+j)];
			dest->data_ptr[4*(i*width+j)+1] 	= Image->data_ptr[3*(i*width+j)+1];
			dest->data_ptr[4*(i*width+j)+2] 	= Image->data_ptr[3*(i*width+j)+2];
			dest->data_ptr[4*(i*width+j)+3] 	= 0;

		}
}
