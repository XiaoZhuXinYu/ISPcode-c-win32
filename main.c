#pragma pack(2)//2字节对齐
#define ISPGLOBAL

#include "include/cj_2dnr.h"
#include "include/cj_debug.h"
#include "include/cj_bmpimage.h"
#include "include/cj_histogram.h"


//static int edges1filter[25] = {
//    -1, 0, 0, 0, 0,
//    0, -2, 0, 0, 0,
//    0, 0, 6, 0, 0,
//    0, 0, 0, -2, 0,
//    0, 0, 0, 0, -1,
//};

static int edges1filter[9] = {
    0, -1, 0,
    -1, 5, -1,
    0, -1, 0
};

int main_bilateraled_median()
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
    readBmpImage("pic/bf3a03.bmp",&testimg);

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

    Convolution2D(testimg.imageData, median.imageData, 640, 480, 1, edges1filter, 3, 1, 0);

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


int main()
{
    printf("start\n");

    long start_time, end_time;

    ImageAttr testimg, heimg,aheimg, clheimg, claheimg;
    testimg.width = 640;
    testimg.height = 480;
    testimg.channels = 1;
    heimg.width = testimg.width;
    heimg.height = testimg.height;
    heimg.channels = testimg.channels;
    aheimg.width = testimg.width;
    aheimg.height = testimg.height;
    aheimg.channels = testimg.channels;
    clheimg.width = testimg.width;
    clheimg.height = testimg.height;
    clheimg.channels = testimg.channels;
    claheimg.width = testimg.width;
    claheimg.height = testimg.height;
    claheimg.channels = testimg.channels;

    testimg.imageData = (unsigned char*)malloc(testimg.width * testimg.height * testimg.channels);
    heimg.imageData = (unsigned char*)malloc(heimg.width * heimg.height * heimg.channels);
    aheimg.imageData = (unsigned char*)malloc(aheimg.width * aheimg.height * aheimg.channels);
    clheimg.imageData = (unsigned char*)malloc(clheimg.width * clheimg.height * clheimg.channels);
    claheimg.imageData = (unsigned char*)malloc(claheimg.width * claheimg.height * claheimg.channels);
    readBmpImage("pic/equalize/pic_0.bmp",&testimg);

    start_time = cj_gettime("ms");
//    cj_HE(&testimg, &heimg);
//    cj_AHE(&testimg, &aheimg, 8);
//    cj_CLHE(&testimg, &clheimg, 1200);
    cj_CLAHE(&testimg, &claheimg, 8, 40.0);
    end_time = cj_gettime("ms");
    printf("time:%ld\n",end_time-start_time);

    SaveBmpImage("pic/equalize/pic_clahe.bmp", &claheimg);

    free(testimg.imageData);
    free(heimg.imageData);
    free(aheimg.imageData);
    free(clheimg.imageData);
    free(claheimg.imageData);

    printf("end\n");
    return 0;
}
