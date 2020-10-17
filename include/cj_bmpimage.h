#ifndef BMPIMAGE_H
#define BMPIMAGE_H
#include <stdio.h>
#include <stdlib.h>

/* 定义WORD为两个字节的类型 */
typedef unsigned short WORD;
/* 定义DWORD为四个字节的类型 */
typedef unsigned int DWORD;

/* 位图文件头主要是对位图文件的一些描述 位图信息头主要是对位图图像方面信息的描述 */
/*************************** 位图标准信息(54字节) ******************************************/
/* 位图文件头 （位图头的字节数 = 位图文件字节数 - 位图图像数据字节数）14 字节 */

#pragma pack(2)//2字节对齐
typedef struct BMP_FILE_HEADER
{
    WORD bType;      // 文件标识符 十六进制为0x4d42,十进制为19778，否则不是bmp格式文件(0-1字节)
    DWORD bSize;     // 文件的大小(2-5字节)
    WORD bReserved1; // 保留值,必须设置为0(6-7字节)
    WORD bReserved2; // 保留值,必须设置为0(8-9字节)
    DWORD bOffset;   // 文件头的最后到图像数据位开始的偏移量(10-13字节)
} BMPFILEHEADER;

/* 位图信息头 40 字节 */
typedef struct BMP_INFO
{
    DWORD bInfoSize;            // 信息头(就是这个结构体本身)的大小(14-17字节)
    DWORD bWidth;               // 图像的宽度(18-21字节)
    DWORD bHeight;              // 图像的高度(22-25字节)
    WORD bPlanes;               // 图像的位面数,显然显示器只有一个平面，所以恒等于1 (26-27字节)
    WORD bBitCount;             // 每个像素的位数,一般为24(28-29字节)
    DWORD bCompression;         // 压缩类型,0为不压缩。 (30-33字节)
    DWORD bmpImageSize;         // 图像的大小,以字节为单位,这个值应该等于上面文件头结构中bSize-bOffset(34-37字节)
    DWORD bXPelsPerMeter;       // 水平分辨率,用象素/米表示。一般为0 (38-41字节)
    DWORD bYPelsPerMeter;       // 垂直分辨率,用象素/米表示。一般为0 (42-45字节)
    DWORD bClrUsed;             // 使用的色彩数,设为0的话，则说明使用所有调色板项(46-49字节)
    DWORD bClrImportant;        // 重要的颜色数,如果是0，表示都重要。(50-53字节)
} BMPINFHEADER;

/* 彩色表:调色板 */
typedef struct RGB_QUAD
{
    unsigned char rgbBlue;     // 蓝色强度
    unsigned char rgbGreen;    // 绿色强度
    unsigned char rgbRed;      // 红色强度
    unsigned char rgbReversed; // 保留值,必须为0
} RGBQUAD;

typedef struct
{
    unsigned int width;
    unsigned int height;
    unsigned int channels;
    unsigned char* imageData;
}ImageAttr;

void readBmpImage(const char *path, ImageAttr* bmpImg);
void showBmpFileHead(void);
void showBmpInfoHead(void);
char SaveBmpImage(char* path, ImageAttr* bmpImg);
#endif // BMPIMAGE_H
