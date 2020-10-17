#pragma pack(2)//2字节对齐
#include "include/cj_histogram.h"

/*
CLAHE:Contrast Limited Adaptive Histogram Equalization
HE算法        直方图均衡
AHE算法       自适应直方图均衡
CLHE算法      限制对比度的直方图均衡
CLAHE算法     限制对比度的自适应直方图均衡
*/

cj_void cj_HE(const ImageAttr* srcImg, ImageAttr* dstImg)
{//pc 上 640 * 480 图片运行时间为2ms。
    cj_u32 u32width;//图像宽度
    cj_u32 u32height;//图像高度
    cj_u32 u32channels;//图像通道数
    cj_u32 u32index;//像素的当前位置
    cj_u32 i,j;//用于计数统计
    cj_u32 au32hist[256]={0};//统计直方图各个灰度值的计数
    cj_u32 au32cumulhist[256]={0};//累计直方图各个灰度值的计数
    cj_u32 au32equalize[256]={0};//归一化后各个灰度值对应新的灰度值
    cj_u32 u32minGrayValue=255;//图像中最小的灰度值
    cj_u32 u32maxGrayValue=0;//图像中最大的灰度值
    cj_u32 u32sum;//用于累计直方图计数

    u32sum = 0;
    u32width = srcImg->width;
    u32height = srcImg->height;
    u32channels = srcImg->channels;

    if(u32channels > 1)
    {
        printf("Please input a grayscale image\n");
        return;
    }

    //统计每个灰度级像素点的个数 统计最小与最大灰度值
    for(i=0; i<u32height; i++)
    {
        for(j=0; j<u32width; j++)
        {
            u32index = i * u32width + j;
            au32hist[srcImg->imageData[u32index]]++;
            u32maxGrayValue = srcImg->imageData[u32index] > u32maxGrayValue ? srcImg->imageData[u32index] : u32maxGrayValue;
            u32minGrayValue = srcImg->imageData[u32index] < u32minGrayValue ? srcImg->imageData[u32index] : u32minGrayValue;
        }
    }
//    printf("minGrayValue = %d ,maxGrayValue = %d\n",minGrayValue,maxGrayValue);//输出最大与最小灰度值

    //累计直方图计数,以及归一化后各个灰度值对应新的灰度值。
    for(i=u32minGrayValue; i<(u32maxGrayValue+1); i++)
    {
//        printf("%d\n", hist[i]);
        u32sum += au32hist[i];
        au32cumulhist[i] += u32sum;
        if(u32maxGrayValue == u32minGrayValue)
        {
            au32equalize[i] = (cj_u32)((cj_float)(u32maxGrayValue * au32cumulhist[i] / (u32width * u32height)) + 0.5f) ;
        }
        else
        {
            au32equalize[i] = (cj_u32)((cj_float)((u32maxGrayValue - u32minGrayValue) * au32cumulhist[i] / (u32width * u32height)) + 0.5f) ;
        }
    }

    //将新的灰度值写到新的图像的对应位置
    for(i=0; i<u32height; i++)
    {
        for(j=0; j<u32width; j++)
        {
            u32index = i * u32width + j;
            dstImg->imageData[u32index] = (cj_u8)au32equalize[srcImg->imageData[u32index]];
        }
    }

    return;
}


cj_void cj_AHE(const ImageAttr* srcImg, ImageAttr* dstImg, cj_u32 u32step)
{//pc 上 640 * 480 图片运行时间为4ms。
    cj_u32 u32width;//图像宽度
    cj_u32 u32height;//图像高度
    cj_u32 u32channels;//图像通道数
    cj_u32 u32block;
    cj_u32 u32block_width; //每个小格子的宽度
    cj_u32 u32block_height;//每个小格子的高度
    cj_u32 u32start_x;//每个块的起始x坐标
    cj_u32 u32end_x;//每个块的结束x坐标
    cj_u32 u32start_y;//每个块的起始y坐标
    cj_u32 u32end_y;//每个块的结束y坐标
    cj_u32 u32num;//当前是第几个块
    cj_u32 u32index;//当前像素点的位置

    cj_u32 i,j,ii,jj;//用于计数统计
    cj_u32** ppu32hist;//统计每个块直方图，各个灰度值的计数
    cj_u32** ppu32cumulhist;//每个块累计直方图计数
    cj_u8** ppu8equalize;//每个块归一化后，各个灰度值对应新的灰度值
    cj_u8* pu8minGrayValue;//每个块 图像中最小的灰度值
    cj_u8* pu8maxGrayValue;//每个块 图像中最大的灰度值
    cj_u32* pu32sum;//每个块 用于累计直方图计数


    u32width = srcImg->width;
    u32height = srcImg->height;
    u32channels = srcImg->channels;
    u32block = u32step;
    u32block_width = u32width / u32block;
    u32block_height = u32height / u32block;

    if(u32channels > 1)
    {
        printf("Please input a grayscale image\n");
        return;
    }

    //内存分配
    ppu32hist = (cj_u32 **)malloc(sizeof(cj_u32 *) * (u32block * u32block)); // 分配所有行的首地址
    ppu32cumulhist = (cj_u32 **)malloc(sizeof(cj_u32 *) * (u32block * u32block)); // 分配所有行的首地址
    ppu8equalize = (cj_u8 **)malloc(sizeof(cj_u8 *) * (u32block * u32block)); // 分配所有行的首地址
    for (i = 0; i < (u32block * u32block); i++)
    { // 按行分配每一列
        ppu32hist[i] = (cj_u32 *)calloc(256, sizeof(cj_u32));
        ppu32cumulhist[i] = (cj_u32 *)calloc(256, sizeof(cj_u32));
        ppu8equalize[i] = (cj_u8 *)calloc(256, sizeof(cj_u8));
    }

    pu8minGrayValue = (cj_u8 *)malloc(sizeof(cj_u8) * (u32block * u32block));
    memset(pu8minGrayValue, 255, (u32block * u32block));
    pu8maxGrayValue = (cj_u8 *)calloc((u32block * u32block), sizeof(cj_u8));
    pu32sum = (cj_u32 *)calloc((u32block * u32block), sizeof(cj_u32));

    for (i=0; i<u32block; i++)
    {
        for (j=0; j<u32block; j++)
        {
            u32start_x = j * u32block_width;
            u32end_x = u32start_x + u32block_width;
            u32start_y = i * u32block_height;
            u32end_y = u32start_y + u32block_height;
            u32num = u32block * i + j;

            //统计每个灰度级像素点的个数 统计最小与最大灰度值
            for(ii = u32start_y ; ii < u32end_y ; ii++)
            {
                for(jj = u32start_x ; jj < u32end_x ; jj++)
                {
                    u32index = ii * u32width + jj;
                    ppu32hist[u32num][srcImg->imageData[u32index]]++;
                    pu8maxGrayValue[u32num] = srcImg->imageData[u32index] > pu8maxGrayValue[u32num] ? srcImg->imageData[u32index] : pu8maxGrayValue[u32num];
                    pu8minGrayValue[u32num] = srcImg->imageData[u32index] < pu8minGrayValue[u32num] ? srcImg->imageData[u32index] : pu8minGrayValue[u32num];
                }
            }

            //累计直方图计数,以及归一化后各个灰度值对应新的灰度值。
            for(ii=pu8minGrayValue[u32num]; ii<(pu8maxGrayValue[u32num]+1); ii++)
            {
                pu32sum[u32num] += ppu32hist[u32num][ii];
                ppu32cumulhist[u32num][ii] += pu32sum[u32num];
                if(pu8maxGrayValue[u32num] == pu8minGrayValue[u32num])
                {
                    ppu8equalize[u32num][ii] = (cj_u8)((cj_float)(pu8maxGrayValue[u32num] * ppu32cumulhist[u32num][ii] / (u32block_width * u32block_height)) + 0.5f) ;
                }
                else
                {
                    ppu8equalize[u32num][ii] = (cj_u8)((cj_float)((pu8maxGrayValue[u32num] - pu8minGrayValue[u32num]) * ppu32cumulhist[u32num][ii] / (u32block_width * u32block_height)) + 0.5f) ;
                }
            }

            //将新的灰度值写到新的图像的对应位置
            for(ii=u32start_y; ii<u32end_y; ii++)
            {
                for(jj=u32start_x; jj<u32end_x; jj++)
                {
                    u32index = ii * u32width + jj;
                    dstImg->imageData[u32index] = ppu8equalize[u32num][srcImg->imageData[u32index]];
                }
            }
        }
    }

    //释放内存
    for (i = 0; i < (u32block * u32block); i++)
    {
        free(ppu32hist[i]);
        free(ppu32cumulhist[i]);
        free(ppu8equalize[i]);
    }
    free(ppu32hist);
    free(ppu32cumulhist);
    free(ppu8equalize);
    free(pu8minGrayValue);
    free(pu8maxGrayValue);
    free(pu32sum);

    return;
}


/*
功能说明：
1. 先将图像中像素个数超过u32cnt的像素，个数限制到 limit 个。
2. 再将各个像素值多出来的像素 (au32hist[i] - u32limit) 平均分给每一个像素值。起到直方图均衡化的作用
*/
cj_void cj_CLHE(const ImageAttr* srcImg, ImageAttr* dstImg, cj_u32 u32cnt)
{//pc 上 640 * 480 图片运行时间为3ms。
    cj_u32 u32width;//图像宽度
    cj_u32 u32height;//图像高度
    cj_u32 u32channels;//图像通道数
    cj_u32 u32index;//当前像素点的位置
    cj_u32 i,j,k;//用于计数统计
    cj_u32 au32hist[256]={0};//统计直方图各个灰度值的计数
    cj_u32 au32cumulhist[256]={0};//累计直方图各个灰度值的计数
    cj_u32 au32equalize[256]={0};//归一化后各个灰度值对应新的灰度值
    cj_u32 u32minGrayValue=255;//图像中最小的灰度值
    cj_u32 u32maxGrayValue=0;//图像中最大的灰度值
    cj_u32 u32sum;//用于累计直方图计数

    cj_u32 u32limit;//用于限制对比度
    cj_u32 u32cumuldelta;//累计偏差

    u32sum = 0;
    u32cumuldelta = 0;
    u32width = srcImg->width;
    u32height = srcImg->height;
    u32channels = srcImg->channels;
    u32limit = (u32width * u32height)/(256 * u32cnt);

    if(u32channels > 1)
    {
        printf("Please input a grayscale image\n");
        return;
    }

    //统计每个灰度级像素点的个数 统计最小与最大灰度值
    for(i=0; i<u32height; i++)
    {
        for(j=0; j<u32width; j++)
        {
            u32index = i * u32width + j;
            au32hist[srcImg->imageData[u32index]]++;
            u32maxGrayValue = srcImg->imageData[u32index] > u32maxGrayValue ? srcImg->imageData[u32index] : u32maxGrayValue;
            u32minGrayValue = srcImg->imageData[u32index] < u32minGrayValue ? srcImg->imageData[u32index] : u32minGrayValue;
        }
    }
//    printf("minGrayValue = %d ,maxGrayValue = %d\n",minGrayValue,maxGrayValue);//输出最大与最小灰度值

    //累计直方图计数,以及归一化后各个灰度值对应新的灰度值。
    for(i=u32minGrayValue; i<(u32maxGrayValue+1); i++)
    {
        if(au32hist[i] > u32limit)
        {
            u32cumuldelta += au32hist[i] - u32limit;
            au32hist[i] = u32limit;
        }
    }

    k = (cj_u32)((cj_float)u32cumuldelta/(u32maxGrayValue - u32minGrayValue +1) + 0.5f);//放在外面是为了减少在for循环中多次计算，降低效率。
    for(i=u32minGrayValue; i<(u32maxGrayValue+1); i++)
    {
        au32hist[i] += k;//将多出来的像素个数平均分配到到每个像素值上
        u32sum += au32hist[i];
        au32cumulhist[i] += u32sum;
        if(u32maxGrayValue == u32minGrayValue)
        {
            au32equalize[i] = (cj_u32)((cj_float)(u32maxGrayValue * au32cumulhist[i] / (u32width * u32height)) + 0.5f) ;
        }
        else
        {
            au32equalize[i] = (cj_u32)((cj_float)((u32maxGrayValue - u32minGrayValue) * au32cumulhist[i] / (u32width * u32height)) + 0.5f) ;
        }
    }

    //将新的灰度值写到新的图像的对应位置
    for(i=0; i<u32height; i++)
    {
        for(j=0; j<u32width; j++)
        {
            u32index = i * u32width + j;
            dstImg->imageData[u32index] = (cj_u8)au32equalize[srcImg->imageData[u32index]];
        }
    }

    return;
}


cj_void cj_CLAHE(const ImageAttr* srcImg, ImageAttr* dstImg, cj_u32 u32step, cj_float fcnt)
{//pc 上 640 * 480 图片运行时间为4ms。
    cj_u32 u32width;//图像宽度
    cj_u32 u32height;//图像高度
    cj_u32 u32channels;//图像通道数
    cj_u32 u32block;
    cj_u32 u32block_width; //每个小格子的宽度
    cj_u32 u32block_height;//每个小格子的高度
    cj_u32 u32start_x;//每个块的起始x坐标
    cj_u32 u32end_x;//每个块的结束x坐标
    cj_u32 u32start_y;//每个块的起始y坐标
    cj_u32 u32end_y;//每个块的结束y坐标
    cj_u32 u32num;//当前是第几个块
    cj_u32 u32index;//当前像素点的位置

    cj_u32 i,j,k, ii,jj;//用于计数统计
    cj_u32** ppu32hist;//统计每个块直方图，各个灰度值的计数
    cj_u32** ppu32cumulhist;//每个块累计直方图计数
    cj_u8** ppu8equalize;//每个块归一化后，各个灰度值对应新的灰度值
    cj_u8* pu8minGrayValue;//每个块 图像中最小的灰度值
    cj_u8* pu8maxGrayValue;//每个块 图像中最大的灰度值
    cj_u32* pu32sum;//每个块 用于累计直方图计数

    cj_u32 u32limit;//用于限制对比度
    cj_u32 u32cumuldelta;//累计偏差




    u32width = srcImg->width;
    u32height = srcImg->height;
    u32channels = srcImg->channels;
    u32block = u32step;
    u32block_width = u32width / u32block;
    u32block_height = u32height / u32block;

    u32cumuldelta = 0;
    u32limit = (cj_u32)((u32width * u32height * fcnt)/256);

    if(u32channels > 1)
    {
        printf("Please input a grayscale image\n");
        return;
    }

    //内存分配
    ppu32hist = (cj_u32 **)malloc(sizeof(cj_u32 *) * (u32block * u32block)); // 分配所有行的首地址
    ppu32cumulhist = (cj_u32 **)malloc(sizeof(cj_u32 *) * (u32block * u32block)); // 分配所有行的首地址
    ppu8equalize = (cj_u8 **)malloc(sizeof(cj_u8 *) * (u32block * u32block)); // 分配所有行的首地址
    for (i = 0; i < (u32block * u32block); i++)
    { // 按行分配每一列
        ppu32hist[i] = (cj_u32 *)calloc(256, sizeof(cj_u32));
        ppu32cumulhist[i] = (cj_u32 *)calloc(256, sizeof(cj_u32));
        ppu8equalize[i] = (cj_u8 *)calloc(256, sizeof(cj_u8));
    }

    pu8minGrayValue = (cj_u8 *)malloc(sizeof(cj_u8) * (u32block * u32block));
    memset(pu8minGrayValue, 255, (u32block * u32block));
    pu8maxGrayValue = (cj_u8 *)calloc((u32block * u32block), sizeof(cj_u8));
    pu32sum = (cj_u32 *)calloc((u32block * u32block), sizeof(cj_u32));

    for (i=0; i<u32block; i++)
    {
        for (j=0; j<u32block; j++)
        {
            u32start_x = j * u32block_width;
            u32end_x = u32start_x + u32block_width;
            u32start_y = i * u32block_height;
            u32end_y = u32start_y + u32block_height;
            u32num = u32block * i + j;

            //统计每个灰度级像素点的个数 统计最小与最大灰度值
            for(ii = u32start_y ; ii < u32end_y ; ii++)
            {
                for(jj = u32start_x ; jj < u32end_x ; jj++)
                {
                    u32index = ii * u32width + jj;
                    ppu32hist[u32num][srcImg->imageData[u32index]]++;
                    pu8maxGrayValue[u32num] = srcImg->imageData[u32index] > pu8maxGrayValue[u32num] ? srcImg->imageData[u32index] : pu8maxGrayValue[u32num];
                    pu8minGrayValue[u32num] = srcImg->imageData[u32index] < pu8minGrayValue[u32num] ? srcImg->imageData[u32index] : pu8minGrayValue[u32num];
                }
            }

            //累计直方图计数,以及归一化后各个灰度值对应新的灰度值。
            u32cumuldelta = 0;
//            printf("u32num = %d, pu8minGrayValue = %d, pu8maxGrayValue %d\n", u32num, pu8minGrayValue[u32num], pu8maxGrayValue[u32num]);
            for(ii=pu8minGrayValue[u32num]; ii<(pu8maxGrayValue[u32num]+1); ii++)
            {
                if(ppu32hist[u32num][ii] > u32limit)
                {
                    u32cumuldelta += ppu32hist[u32num][ii] - u32limit;
                    ppu32hist[u32num][ii] = u32limit;
                }
            }

            k = (cj_u32)((cj_float)u32cumuldelta/(pu8maxGrayValue[u32num] - pu8minGrayValue[u32num] +1) + 0.5f);//放在外面是为了减少在for循环中多次计算，降低效率。
            for(ii=pu8minGrayValue[u32num]; ii<(pu8maxGrayValue[u32num]+1); ii++)
            {
                ppu32hist[u32num][ii] += k;//将多出来的像素个数平均分配到到每个像素值上
                pu32sum[u32num] += ppu32hist[u32num][ii];
                ppu32cumulhist[u32num][ii] += pu32sum[u32num];
                if(pu8maxGrayValue[u32num] == pu8minGrayValue[u32num])
                {
                    ppu8equalize[u32num][ii] = (cj_u8)((cj_float)(pu8maxGrayValue[u32num] * ppu32cumulhist[u32num][ii] / (u32block_width * u32block_height)) + 0.5f) ;
                }
                else
                {
                    ppu8equalize[u32num][ii] = (cj_u8)((cj_float)((pu8maxGrayValue[u32num] - pu8minGrayValue[u32num]) * ppu32cumulhist[u32num][ii] / (u32block_width * u32block_height)) + 0.5f) ;
                }
            }
        }
    }

    //计算变换后的像素值  根据像素点的位置，选择不同的计算方法
    for(i = 0 ; i < u32height; i++)
    {
        for(j = 0 ; j < u32width; j++)
        {
            //four coners
            u32index = i * u32width + j;
            if(i < u32block_height/2  && j < u32block_width/2)
            {
                u32num = 0;
                dstImg->imageData[u32index] = ppu8equalize[u32num][srcImg->imageData[u32index]];
            }
            else if(i >= ((u32block-1)*u32block_height + u32block_height/2) && j <= u32block_width/2)
            {
                u32num = u32block*(u32block-1);
                dstImg->imageData[u32index] = ppu8equalize[u32num][srcImg->imageData[u32index]];
            }
            else if(i < u32block_height/2 && j >= ((u32block-1)*u32block_width+u32block_width/2))
            {
                u32num = u32block-1;
                dstImg->imageData[u32index] = ppu8equalize[u32num][srcImg->imageData[u32index]];
            }
            else if(i >= ((u32block-1) * u32block_height + u32block_height / 2) && j >= ((u32block - 1) * u32block_width + u32block_width / 2))
            {
                u32num = u32block * u32block - 1;
                dstImg->imageData[u32index] = ppu8equalize[u32num][srcImg->imageData[u32index]];
            }
            //four edges except coners
            else if(j < u32block_width / 2)
            {
                //线性插值
                cj_u32 num_i = (i - u32block_height/2) / u32block_height;
                cj_u32 num_j = 0;
                cj_u32 num1 = num_i*u32block + num_j;
                cj_u32 num2 = num1 + u32block;
                cj_float p =  (i - (num_i*u32block_height+u32block_height/2))/(1.0f*u32block_height);
                cj_float q = 1-p;
                dstImg->imageData[u32index] = (cj_u8)(q * ppu8equalize[num1][srcImg->imageData[u32index]]+ p * ppu8equalize[num2][srcImg->imageData[u32index]]);
            }
            else if(j >= ((u32block-1)*u32block_width+u32block_width/2))
            {
                //线性插值
                cj_u32 num_i = (i - u32block_height/2)/u32block_height;
                cj_u32 num_j = u32block-1;
                cj_u32 num1 = num_i*u32block + num_j;
                cj_u32 num2 = num1 + u32block;
                cj_float p =  (i - (num_i*u32block_height+u32block_height/2))/(1.0f*u32block_height);
                cj_float q = 1-p;
                dstImg->imageData[u32index] = (cj_u8)(q * ppu8equalize[num1][srcImg->imageData[u32index]]+ p * ppu8equalize[num2][srcImg->imageData[u32index]]);
            }
            else if(i < u32block_height/2)
            {
                //线性插值
                cj_u32 num_i = 0;
                cj_u32 num_j = (j - u32block_width/2)/u32block_width;
                cj_u32 num1 = num_i*u32block + num_j;
                cj_u32 num2 = num1 + 1;
                cj_float p =  (j - (num_j*u32block_width+u32block_width/2))/(1.0f*u32block_width);
                cj_float q = 1-p;
                dstImg->imageData[u32index] = (cj_u8)(q * ppu8equalize[num1][srcImg->imageData[u32index]]+ p * ppu8equalize[num2][srcImg->imageData[u32index]]);
            }
            else if(i >= ((u32block-1)*u32block_height + u32block_height/2))
            {
                //线性插值
                cj_u32 num_i = u32block-1;
                cj_u32 num_j = (j - u32block_width/2)/u32block_width;
                cj_u32 num1 = num_i*u32block + num_j;
                cj_u32 num2 = num1 + 1;
                cj_float p =  (j - (num_j*u32block_width+u32block_width/2))/(1.0f*u32block_width);
                cj_float q = 1-p;
                dstImg->imageData[u32index] = (cj_u8)(q * ppu8equalize[num1][srcImg->imageData[u32index]]+ p * ppu8equalize[num2][srcImg->imageData[u32index]]);
            }
            else
            {
                //双线性插值
                cj_u32 num_i = (i - u32block_height/2)/u32block_height;
                cj_u32 num_j = (j - u32block_width/2)/u32block_width;
                cj_u32 num1 = num_i*u32block + num_j;
                cj_u32 num2 = num1 + 1;
                cj_u32 num3 = num1 + u32block;
                cj_u32 num4 = num2 + u32block;
                cj_float u = (j - (num_j*u32block_width+u32block_width/2))/(1.0f*u32block_width);
                cj_float v = (i - (num_i*u32block_height+u32block_height/2))/(1.0f*u32block_height);

                dstImg->imageData[u32index] = (cj_u8)(((1-u) * (1-v) * ppu8equalize[num1][srcImg->imageData[u32index]] +
                                                        u * (1-v) * ppu8equalize[num2][srcImg->imageData[u32index]] +
                                                       (1-u) * v * ppu8equalize[num3][srcImg->imageData[u32index]] +
                                                       u * v * ppu8equalize[num4][srcImg->imageData[u32index]]));
            }
        }
    }

    //释放内存
    for (i = 0; i < (u32block * u32block); i++)
    {
        free(ppu32hist[i]);
        free(ppu32cumulhist[i]);
        free(ppu8equalize[i]);
    }
    free(ppu32hist);
    free(ppu32cumulhist);
    free(ppu8equalize);
    free(pu8minGrayValue);
    free(pu8maxGrayValue);
    free(pu32sum);

    return;
}

