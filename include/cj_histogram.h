#ifndef CJ_HISTOGRAM_H
#define CJ_HISTOGRAM_H


#include "include/cj_includes.h"

#pragma pack(2)//2字节对齐
#include "include/cj_bmpimage.h"

cj_void cj_HE(const ImageAttr* srcImg, ImageAttr* dstImg);
cj_void cj_AHE(const ImageAttr* srcImg, ImageAttr* dstImg, cj_u32 u32step);
cj_void cj_CLHE(const ImageAttr* srcImg, ImageAttr* dstImg, cj_u32 u32cnt);
cj_void cj_CLAHE(const ImageAttr* srcImg, ImageAttr* dstImg, cj_u32 u32step, cj_float fcnt);
#endif // CJ_HISTOGRAM_H
