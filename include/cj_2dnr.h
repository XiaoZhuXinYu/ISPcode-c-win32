#ifndef CJ_2DNR_H
#define CJ_2DNR_H
#include "stdio.h"

int getDiffFactor(const unsigned char* color1, const unsigned char* color2, const  unsigned int channels);
void CRB_HorizontalFilter(unsigned char* Input, unsigned char* Output, unsigned int Width, unsigned int Height, unsigned int Channels, float * range_table_f, float inv_alpha_f, float* left_Color_Buffer, float* left_Factor_Buffer, float* right_Color_Buffer, float* right_Factor_Buffer);
void CRB_VerticalFilter(unsigned char* Input, unsigned char* Output, unsigned int Width, unsigned int Height, unsigned int Channels, float * range_table_f, float inv_alpha_f, float* down_Color_Buffer, float* down_Factor_Buffer, float* up_Color_Buffer, float* up_Factor_Buffer);
void CRBFilter(unsigned char* Input, unsigned char* Output, unsigned int Width, unsigned int Height, unsigned int Stride, float sigmaSpatial, float sigmaRange);
void bilateral_filter_gray(unsigned char* Input, unsigned char* Output, unsigned int Width, unsigned int Height, unsigned char diameter, float sigmaColor, float sigmaSpace);
void bilateral_filter_gray1(unsigned char* Input, unsigned char* Output, unsigned int Width, unsigned int Height, unsigned char diameter, float sigmaColor, float sigmaSpace);

void fastMedianBlur_gray(const unsigned char* srcImg, unsigned char* dstImg, unsigned int Width, unsigned int Height, unsigned int diameter);
void normalMedianBlur_gray(const unsigned char* srcImg, unsigned char* dstImg, unsigned int Width, unsigned int Height, unsigned int diameter, char* mode);
void Convolution2D(unsigned char * data, unsigned char* dstImg, unsigned int width, unsigned int height,
                   unsigned int channels, int * filter, unsigned char filterW, unsigned char cfactor, unsigned char bias);
void Convolution2D1(unsigned char * data, unsigned char* dstImg, unsigned int width, unsigned int height,
                   unsigned int channels, int * filter, unsigned char filterW, unsigned char cfactor, unsigned char bias);
#endif // CJ_2DNR_H

