#pragma pack(2)//2字节对齐
#include <stdio.h>
#include "include/cj_2dnr.h"
#include "include/cj_debug.h"
#include "include/bmpimage.h"


static int edges1filter[25] = {
    -1, 0, 0, 0, 0,
    0, -2, 0, 0, 0,
    0, 0, 6, 0, 0,
    0, 0, 0, -2, 0,
    0, 0, 0, 0, -1,
};

int main()
{
    long start_time, end_time;
//    printf("start\n");

    ImageAttr testimg, bilateraled, median;
    testimg.width = 640;
    testimg.height = 480;
    testimg.channels = 1;
    bilateraled.width = 640;
    bilateraled.height = 480;
    bilateraled.channels = 1;
    median.width = 640;
    median.height = 480;
    median.channels = 1;
    testimg.imageData = (unsigned char*)malloc(testimg.width*testimg.height*testimg.channels);
    bilateraled.imageData = (unsigned char*)malloc(bilateraled.width*bilateraled.height*bilateraled.channels);
    median.imageData = (unsigned char*)malloc(median.width*median.height*median.channels);
    readBmpImage("pic/bf3a03.bmp",testimg.imageData);

    start_time = cj_gettime("ms");

//    CRBFilter(testimg.imageData, bilateraled.imageData, bilateraled.width, bilateraled.height, bilateraled.width, 0.1f, 0.1f);
//    bilateral_filter_gray(testimg.imageData, bilateraled.imageData, testimg.width, testimg.height, 3, 3.0, 3.0);

//    fastMedianBlur_gray(testimg.imageData, median.imageData, median.width, median.height, 3);
//    normalMedianBlur_gray(testimg.imageData, median.imageData, median.width, median.height, 5, "bubble");
//    normalMedianBlur_gray(testimg.imageData, median.imageData, median.width, median.height, 5, "select");
//    normalMedianBlur_gray(testimg.imageData, median.imageData, median.width, median.height, 3, "insert");
//    normalMedianBlur_gray(testimg.imageData, median.imageData, median.width, median.height, 5, "shell");
//    normalMedianBlur_gray(testimg.imageData, median.imageData, median.width, median.height, 5, "quick");
//    normalMedianBlur_gray(testimg.imageData, median.imageData, median.width, median.height, 5, "merge");
//    normalMedianBlur_gray(testimg.imageData, median.imageData, median.width, median.height, 5, "heap");

    Convolution2D(testimg.imageData, median.imageData, 640, 480, 1, edges1filter, 5, 1, 0);

    end_time = cj_gettime("ms");
    printf("time:%ld\n",end_time-start_time);

//    SaveBmpImage("pic/lena_gray_denoised.bmp", &bilateraled);
    SaveBmpImage("pic/lena_gray_median.bmp", &median);

    free(testimg.imageData);
    free(bilateraled.imageData);
    free(median.imageData);
//    printf("end\n");


    return 0;
}
