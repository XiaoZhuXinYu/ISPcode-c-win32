#pragma pack(2)//2字节对齐
#include "include/cj_bmpimage.h"
#include "string.h"

static BMPFILEHEADER fileHeader;
static BMPINFHEADER infoHeader;

void readBmpImage(const char *path, ImageAttr* bmpImg)
{
    FILE *fp;
    unsigned int imagesize = 0;
    unsigned char* imagetmp = NULL;
    fp = fopen(path, "r");
    if(fp == NULL) // fp = 0x00426aa0
    {
        printf("Cann't open the file!\n");
        return;
    }
    fseek(fp, 0, SEEK_SET);//文件指针会指向文件的开始
    fread(&fileHeader, sizeof(BMPFILEHEADER), 1, fp);
    fread(&infoHeader, sizeof(BMPINFHEADER), 1, fp);

//    showBmpFileHead();
//    showBmpInfoHead();

    if(fileHeader.bSize)
    {
        imagesize = fileHeader.bSize - fileHeader.bOffset;
    }
    else
    {//有些不正规的bmp文件fileHeader.bSize为0。目前就遇到这些，若有其他情况遇到再说吧。
        imagesize = infoHeader.bWidth * infoHeader.bHeight * infoHeader.bBitCount / 8;
    }
    fseek(fp, (long)fileHeader.bOffset, SEEK_SET);//跳转至数据区
    imagetmp = (unsigned char*)malloc(imagesize);
    if(imagetmp==NULL)
    {
        printf("malloc error\n");
        free(imagetmp);
    }

    fread(imagetmp, 1, imagesize, fp);
    memcpy(bmpImg->imageData, imagetmp, imagesize);

    free(imagetmp);
    return;
}

void showBmpFileHead(void)
{  //定义显示信息的函数，传入文件头结构体
    printf("image type:0x%x\n", fileHeader.bType);
    printf("image size:%d\n", fileHeader.bSize);
    printf("image bReserved1:%d\n",  fileHeader.bReserved1);
    printf("image bReserved2:%d\n",  fileHeader.bReserved2);
    printf("image Offset:%d\n",  fileHeader.bOffset);
}

void showBmpInfoHead(void)
{//定义显示信息的函数，传入的是信息头结构体
   printf("image InfoSize:%d\n" ,infoHeader.bInfoSize);
   printf("image Width:%d\n" ,infoHeader.bWidth);
   printf("image Height:%d\n" ,infoHeader.bHeight);
   printf("image Planes:%d\n" ,infoHeader.bPlanes);
   printf("image BitCount:%d\n" ,infoHeader.bBitCount);
   printf("image Compression:%d\n" ,infoHeader.bCompression);
   printf("imagesize:%d\n" ,infoHeader.bmpImageSize);
   printf("image PelsPerMete:%d\n" ,infoHeader.bXPelsPerMeter);
   printf("image YPelsPerMeter:%d\n" ,infoHeader.bYPelsPerMeter);
   printf("image ClrUsed:%d\n" ,infoHeader.bClrUsed);
   printf("image ClrImportant:%d\n" ,infoHeader.bClrImportant);
}

char SaveBmpImage(char* path, ImageAttr* bmpImg)
{
    FILE *pFile;
    BMPFILEHEADER bmpFileHeader;
    BMPINFHEADER bmpInfoHeader;
    unsigned int step;
    unsigned int offset;
    unsigned char pixVal = '\0';
    unsigned int i, j;
    RGBQUAD* quad;

    pFile = fopen(path, "wb");
    if (!pFile)
    {
        printf("open file error\n");
        return 0;
    }

    if (bmpImg->channels == 3)//24位，通道，彩图
    {
        step = bmpImg->channels*bmpImg->width;
        offset = step%4;
        if (offset != 0)
        {
            step += 4-offset;
        }
        bmpFileHeader.bType = 0x4D42;
        bmpFileHeader.bSize = bmpImg->height*step + 54;
        bmpFileHeader.bReserved1 = 0;
        bmpFileHeader.bReserved2 = 0;
        bmpFileHeader.bOffset = 54;
        fwrite(&bmpFileHeader, sizeof(BMPFILEHEADER), 1, pFile);

        bmpInfoHeader.bInfoSize = 40;
        bmpInfoHeader.bWidth = bmpImg->width;
        bmpInfoHeader.bHeight = bmpImg->height;
        bmpInfoHeader.bPlanes = 1;
        bmpInfoHeader.bBitCount = 24;
        bmpInfoHeader.bCompression = 0;
        bmpInfoHeader.bmpImageSize = bmpImg->height*step;
//        bmpInfoHeader.bmpImageSize = 0;//实际读取的24位bmp图片，size数为0
        bmpInfoHeader.bXPelsPerMeter = 0;
        bmpInfoHeader.bYPelsPerMeter = 0;
        bmpInfoHeader.bClrUsed = 0;
        bmpInfoHeader.bClrImportant = 0;
        fwrite(&bmpInfoHeader, sizeof(BMPINFHEADER), 1, pFile);

        for (i=0; i<bmpImg->height; i++)//数据和实际显示上下镜像？？
        {
            for (j=0; j<bmpImg->width; j++)
            {//rgb顺序从低字节到高字节依次为bgr
                pixVal = bmpImg->imageData[i*bmpImg->width*3+j*3];
                fwrite(&pixVal, sizeof(unsigned char), 1, pFile);
                pixVal = bmpImg->imageData[i*bmpImg->width*3+j*3+1];
                fwrite(&pixVal, sizeof(unsigned char), 1, pFile);
                pixVal = bmpImg->imageData[i*bmpImg->width*3+j*3+2];
                fwrite(&pixVal, sizeof(unsigned char), 1, pFile);
            }
            if (offset!=0)
            {
                for (j=0; j<offset; j++)
                {
                    pixVal = 0;
                    fwrite(&pixVal, sizeof(unsigned char), 1, pFile);
                }
            }
        }
    }
    else if (bmpImg->channels == 1)//8位，单通道，灰度图
    {
        step = bmpImg->width;
        offset = step%4;
        if (offset != 0)
        {
            step += 4-offset;
        }

        bmpFileHeader.bType = 0x4d42;
        bmpFileHeader.bSize = 54 + 256*4 + step * bmpImg->height;
        bmpFileHeader.bReserved1 = 0;
        bmpFileHeader.bReserved2 = 0;
        bmpFileHeader.bOffset = 54 + 256*4;
        fwrite(&bmpFileHeader, sizeof(BMPFILEHEADER), 1, pFile);

        bmpInfoHeader.bInfoSize = 40;
        bmpInfoHeader.bWidth = bmpImg->width;
        bmpInfoHeader.bHeight = bmpImg->height;
        bmpInfoHeader.bPlanes = 1;
        bmpInfoHeader.bBitCount = 8;
        bmpInfoHeader.bCompression = 0;
        bmpInfoHeader.bmpImageSize = step * bmpImg->height;
//        bmpInfoHeader.bmpImageSize = 0;//实际的8位bmp图这个数据位0
        bmpInfoHeader.bXPelsPerMeter = 0;
        bmpInfoHeader.bYPelsPerMeter = 0;
        bmpInfoHeader.bClrUsed = 256;
        bmpInfoHeader.bClrImportant = 256;
        fwrite(&bmpInfoHeader, sizeof(BMPINFHEADER), 1, pFile);

        quad = (RGBQUAD*)malloc(sizeof(RGBQUAD)*256);
        for (i=0; i<256; i++)
        {
            quad[i].rgbBlue = (unsigned char)i;
            quad[i].rgbGreen = (unsigned char)i;
            quad[i].rgbRed = (unsigned char)i;
            quad[i].rgbReversed = 0;
        }
        fwrite(quad, sizeof(RGBQUAD), 256, pFile);
        free(quad);

        for (i=0; i<bmpImg->height; i++)
        {
            for (j=0; j<bmpImg->width; j++)
            {
                pixVal = bmpImg->imageData[i*bmpImg->width+j];
                fwrite(&pixVal, sizeof(unsigned char), 1, pFile);
            }
            if (offset!=0)
            {
                for (j=0; j<offset; j++)
                {
                    pixVal = 0;
                    fwrite(&pixVal, sizeof(unsigned char), 1, pFile);
                }
            }
        }
    }
    fclose(pFile);

    return 0;
}
