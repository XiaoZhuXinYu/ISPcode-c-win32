//#include "stdio.h"
#include "math.h"
#include "stdlib.h"
#include "string.h"
#include "include/cj_2dnr.h"
#include "include/cj_base_alg.h"

int getDiffFactor(const unsigned char* color1, const unsigned char* color2, const  unsigned int channels)
{
    int final_diff;
    int component_diff[4];
    unsigned int i;
    // find absolute difference between each component
    for (i = 0; i < channels; i++)
    {
        component_diff[i] = abs(color1[i] - color2[i]);
    }

    // based on number of components, produce a single difference value in the 0-255 range
    switch (channels)
    {
    case 1:
        final_diff = component_diff[0];
        break;

    case 2:
        final_diff = ((component_diff[0] + component_diff[1]) >> 1);
        break;

    case 3:
        final_diff = ((component_diff[0] + component_diff[2]) >> 2) + (component_diff[1] >> 1);
        break;

    case 4:
        final_diff = ((component_diff[0] + component_diff[1] + component_diff[2] + component_diff[3]) >> 2);
        break;

    default:
        final_diff = 0;
    }

//    _ASSERT(final_diff >= 0 && final_diff <= 255);

    return final_diff;
}

void CRB_HorizontalFilter(unsigned char* Input, unsigned char* Output, unsigned int Width, unsigned int Height, unsigned int Channels, float * range_table_f, float inv_alpha_f, float* left_Color_Buffer, float* left_Factor_Buffer, float* right_Color_Buffer, float* right_Factor_Buffer)
{

    // Left pass and Right pass

    unsigned int Stride = Width * Channels;
    const unsigned char* src_left_color = Input;
    float* left_Color = left_Color_Buffer;
    float* left_Factor = left_Factor_Buffer;

    unsigned int last_index = Stride * Height - 1;
    const unsigned char* src_right_color = Input + last_index;
    float* right_Color = right_Color_Buffer + last_index;
    float* right_Factor = right_Factor_Buffer + Width * Height - 1;

    for (unsigned int y = 0; y < Height; y++)
    {
        const unsigned char* src_left_prev = Input;
        const float* left_prev_factor = left_Factor;
        const float* left_prev_color = left_Color;

        const unsigned char* src_right_prev = src_right_color;
        const float* right_prev_factor = right_Factor;
        const float* right_prev_color = right_Color;

        // process 1st pixel separately since it has no previous
        {
            //if x = 0
            *left_Factor++ = 1.f;
            *right_Factor-- = 1.f;
            for (unsigned int c = 0; c < Channels; c++)
            {
                *left_Color++ = *src_left_color++;
                *right_Color-- = *src_right_color--;
            }
        }
        // handle other pixels
        for (unsigned int x = 1; x < Width; x++)
        {
            // determine difference in pixel color between current and previous
            // calculation is different depending on number of channels
            int left_diff = getDiffFactor(src_left_color, src_left_prev, Channels);
            src_left_prev = src_left_color;

            int right_diff = getDiffFactor(src_right_color, src_right_color - Channels, Channels);
            src_right_prev = src_right_color;

            float left_alpha_f = range_table_f[left_diff];
            float right_alpha_f = range_table_f[right_diff];
            *left_Factor++ = inv_alpha_f + left_alpha_f * (*left_prev_factor++);
            *right_Factor-- = inv_alpha_f + right_alpha_f * (*right_prev_factor--);

            for (unsigned int c = 0; c < Channels; c++)
            {
                *left_Color++ = (inv_alpha_f * (*src_left_color++) + left_alpha_f * (*left_prev_color++));
                *right_Color-- = (inv_alpha_f * (*src_right_color--) + right_alpha_f * (*right_prev_color--));
            }
        }
    }
    // vertical pass will be applied on top on horizontal pass, while using pixel differences from original image
    // result color stored in 'leftcolor' and vertical pass will use it as source color
    {
        unsigned char* dst_color = Output; // use as temporary buffer
        const float* leftcolor = left_Color_Buffer;
        const float* leftfactor = left_Factor_Buffer;
        const float* rightcolor = right_Color_Buffer;
        const float* rightfactor = right_Factor_Buffer;

        unsigned int width_height = Width * Height;
        for (unsigned int i = 0; i < width_height; i++)
        {
            // average color divided by average factor
            float factor = 1.f / ((*leftfactor++) + (*rightfactor++));
            for (unsigned int c = 0; c < Channels; c++)
            {
                *dst_color++ = (unsigned char)(factor * ((*leftcolor++) + (*rightcolor++)));
            }
        }
    }
}

void CRB_VerticalFilter(unsigned char* Input, unsigned char* Output, unsigned int Width, unsigned int Height, unsigned int Channels, float * range_table_f, float inv_alpha_f, float* down_Color_Buffer, float* down_Factor_Buffer, float* up_Color_Buffer, float* up_Factor_Buffer)
{

    // Down pass and Up pass
    unsigned int Stride = Width * Channels;
    const unsigned char* src_color_first_hor = Output; // result of horizontal pass filter
    const unsigned char* src_down_color = Input;
    float* down_color = down_Color_Buffer;
    float* down_factor = down_Factor_Buffer;

    const unsigned char* src_down_prev = src_down_color;
    const float* down_prev_color = down_color;
    const float* down_prev_factor = down_factor;


    unsigned int last_index = Stride * Height - 1;
    const unsigned char* src_up_color = Input + last_index;
    const unsigned char* src_color_last_hor = Output + last_index; // result of horizontal pass filter
    float* up_color = up_Color_Buffer + last_index;
    float* up_factor = up_Factor_Buffer + (Width * Height - 1);

    const float* up_prev_color = up_color;
    const float* up_prev_factor = up_factor;

    // 1st line done separately because no previous line
    {
        //if y=0
        for (unsigned int x = 0; x < Width; x++)
        {
            *down_factor++ = 1.f;
            *up_factor-- = 1.f;
            for (unsigned int c = 0; c < Channels; c++)
            {
                *down_color++ = *src_color_first_hor++;
                *up_color-- = *src_color_last_hor--;
            }
            src_down_color += Channels;
            src_up_color -= Channels;
        }
    }
    // handle other lines
    for (unsigned int y = 1; y < Height; y++)
    {
        for (unsigned int x = 0; x < Width; x++)
        {
            // determine difference in pixel color between current and previous
            // calculation is different depending on number of channels
            int down_diff = getDiffFactor(src_down_color, src_down_prev, Channels);
            src_down_prev += Channels;
            src_down_color += Channels;
            src_up_color -= Channels;
            int up_diff = getDiffFactor(src_up_color, src_up_color + Stride, Channels);
            float down_alpha_f = range_table_f[down_diff];
            float up_alpha_f = range_table_f[up_diff];

            *down_factor++ = inv_alpha_f + down_alpha_f * (*down_prev_factor++);
            *up_factor-- = inv_alpha_f + up_alpha_f * (*up_prev_factor--);

            for (unsigned int c = 0; c < Channels; c++)
            {
                *down_color++ = inv_alpha_f * (*src_color_first_hor++) + down_alpha_f * (*down_prev_color++);
                *up_color-- = inv_alpha_f * (*src_color_last_hor--) + up_alpha_f * (*up_prev_color--);
            }
        }
    }

    // average result of vertical pass is written to output buffer
    {
        unsigned char *dst_color = Output;
        const float* downcolor = down_Color_Buffer;
        const float* downfactor = down_Factor_Buffer;
        const float* upcolor = up_Color_Buffer;
        const float* upfactor = up_Factor_Buffer;

        unsigned int width_height = Width * Height;
        for (unsigned int i = 0; i < width_height; i++)
        {
            // average color divided by average factor
            float factor = 1.f / ((*upfactor++) + (*downfactor++));
            for (unsigned int c = 0; c < Channels; c++)
            {
                *dst_color++ = (unsigned char)(factor * ((*upcolor++) + (*downcolor++)));
            }
        }
    }
}

// memory must be reserved before calling image filter
// this implementation of filter uses plain C++, single threaded
// channel count must be 3 or 4 (alpha not used)
void CRBFilter(unsigned char* Input, unsigned char* Output, unsigned int Width, unsigned int Height, unsigned int Stride, float sigmaSpatial, float sigmaRange)
{
    unsigned int Channels = Stride / Width;
    unsigned int reserveWidth = Width;
    unsigned int reserveHeight = Height;
    // basic sanity check
//    _ASSERT(Input);
//    _ASSERT(Output);
//    _ASSERT(reserveWidth >= 10 && reserveWidth < 10000);
//    _ASSERT(reserveHeight >= 10 && reserveHeight < 10000);
//    _ASSERT(Channels >= 1 && Channels <= 4);

    unsigned int reservePixels = reserveWidth * reserveHeight;
    unsigned int numberOfPixels = reservePixels * Channels;

    float* leftColorBuffer = (float*)calloc(numberOfPixels, sizeof(float));
    float* leftFactorBuffer = (float*)calloc(reservePixels, sizeof(float));
    float* rightColorBuffer = (float*)calloc(numberOfPixels, sizeof(float));
    float* rightFactorBuffer = (float*)calloc(reservePixels, sizeof(float));

    if (leftColorBuffer == NULL || leftFactorBuffer == NULL || rightColorBuffer == NULL || rightFactorBuffer == NULL)
    {
        if (leftColorBuffer)  free(leftColorBuffer);
        if (leftFactorBuffer) free(leftFactorBuffer);
        if (rightColorBuffer) free(rightColorBuffer);
        if (rightFactorBuffer) free(rightFactorBuffer);

        return;
    }
    float* downColorBuffer = leftColorBuffer;
    float* downFactorBuffer = leftFactorBuffer;
    float* upColorBuffer = rightColorBuffer;
    float* upFactorBuffer = rightFactorBuffer;
    // compute a lookup table
    float alpha_f = (float)(exp(-sqrt(2.0) / ((double)sigmaSpatial * 255)));
    float inv_alpha_f = 1.f - alpha_f;


    float range_table_f[255 + 1];
    float inv_sigma_range = 1.0f / (sigmaRange * 255);

    float ii = 0.f;
    for (int i = 0; i <= 255; i++, ii -= 1.f)
    {
        range_table_f[i] = alpha_f * (float)exp((double)(ii * inv_sigma_range));
    }
    CRB_HorizontalFilter(Input, Output, Width, Height, Channels, range_table_f, inv_alpha_f, leftColorBuffer, leftFactorBuffer, rightColorBuffer, rightFactorBuffer);

    CRB_VerticalFilter(Input, Output, Width, Height, Channels, range_table_f, inv_alpha_f, downColorBuffer, downFactorBuffer, upColorBuffer, upFactorBuffer);

    if (leftColorBuffer)
    {
        free(leftColorBuffer);
        leftColorBuffer = NULL;
    }

    if (leftFactorBuffer)
    {
        free(leftFactorBuffer);
        leftFactorBuffer = NULL;
    }

    if (rightColorBuffer)
    {
        free(rightColorBuffer);
        rightColorBuffer = NULL;
    }

    if (rightFactorBuffer)
    {
        free(rightFactorBuffer);
        rightFactorBuffer = NULL;
    }
}



static float gaussian(unsigned int x, float sigma)
{
    return expf(-(float)(x * x) / (2 * sigma * sigma));
}



//static float distance(int x1, int y1, int x2, int y2)
//{
//    return (float)sqrt(abs((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)));
//}


void bilateral_filter_gray(unsigned char* Input, unsigned char* Output, unsigned int Width, unsigned int Height, unsigned char diameter, float sigmaColor, float sigmaSpace)
{
    //    不考虑边缘的情况
    float weight_color[256] = {0};
    float weight_space[256];
    float wp_total, gi, gs, wp, filtered_image;
    unsigned char pixel, data;
    int i,j;
    unsigned int img_height, img_width;
    unsigned int start_row, start_col, end_row, end_col, row, col;
    char radius;
    unsigned char maxk;


    img_height = Height;
    img_width = Width;

//    计算颜色模板系数表
    for(i=0;i<256;i++)
    {
        weight_color[i] = gaussian((unsigned int)i, sigmaColor);
    }


//    计算空间模板
    radius = diameter / 2;
    maxk = 0;
    for(i=-radius;i<radius + 1;i++)
    {
        for(j=-radius;j<radius + 1;j++)
        {
            weight_space[maxk] = gaussian((unsigned int)(i * i + j * j), sigmaSpace);
            maxk = maxk + 1;
        }
    }

    //所有像素进行滤波
    start_row = diameter / 2;
    start_col = diameter / 2;
    end_row = img_height - diameter / 2;
    end_col = img_width - diameter / 2;
    for(row=start_row;row<end_row;row++)
    {
        for(col=start_col; col<end_col; col++)
        {
            wp_total = 1;
            filtered_image = Input[img_width*row+col];
            pixel = Input[img_width*row+col];

//            每个窗口的所有像素
            data = Input[img_width*(row-1)+col-1];
            gi = weight_color[abs((int)(data) - (int)(pixel))];
            gs = weight_space[0];// 边界处值域权重为0，空域不做特殊处理
            wp = gi * gs;
            filtered_image = filtered_image + (data * wp);
            wp_total = wp_total + wp;

            data = Input[img_width*(row-1)+col];
            gi = weight_color[abs((int)(data) - (int)(pixel))];
            gs = weight_space[1];//边界处值域权重为0，空域不做特殊处理
            wp = gi * gs;
            filtered_image = filtered_image + (data * wp);
            wp_total = wp_total + wp;

            data = Input[img_width*(row-1)+col+1];
            gi = weight_color[abs((int)(data) - (int)(pixel))];
            gs = weight_space[2];// 边界处值域权重为0，空域不做特殊处理
            wp = gi * gs;
            filtered_image = filtered_image + (data * wp);
            wp_total = wp_total + wp;

            data = Input[img_width*row+col-1];
            gi = weight_color[abs((int)(data) - (int)(pixel))];
            gs = weight_space[3];// 边界处值域权重为0，空域不做特殊处理
            wp = gi * gs;
            filtered_image = filtered_image + (data * wp);
            wp_total = wp_total + wp;

//            data = Input[img_width*row+col];
//            gi = weight_color[abs((int)(data) - (int)(pixel))];
//            gs = weight_space[4];  # 边界处值域权重为0，空域不做特殊处理
//            wp = gi * gs;
//            filtered_image = filtered_image + (data * wp);
//            wp_total = wp_total + wp;

            data = Input[img_width*row+col+1];
            gi = weight_color[abs((int)(data) - (int)(pixel))];
            gs = weight_space[5]; //边界处值域权重为0，空域不做特殊处理
            wp = gi * gs;
            filtered_image = filtered_image + (data * wp);
            wp_total = wp_total + wp;

            data = Input[img_width*(row+1)+col-1];
            gi = weight_color[abs((int)(data) - (int)(pixel))];
            gs = weight_space[6]; //边界处值域权重为0，空域不做特殊处理
            wp = gi * gs;
            filtered_image = filtered_image + (data * wp);
            wp_total = wp_total + wp;

            data = Input[img_width*(row+1)+col];
            gi = weight_color[abs((int)(data) - (int)(pixel))];
            gs = weight_space[7]; //边界处值域权重为0，空域不做特殊处理
            wp = gi * gs;
            filtered_image = filtered_image + (data * wp);
            wp_total = wp_total + wp;

            data = Input[img_width*(row+1)+col+1];
            gi = weight_color[abs((int)(data) - (int)(pixel))];
            gs = weight_space[8]; //边界处值域权重为0，空域不做特殊处理
            wp = gi * gs;
            filtered_image = filtered_image + (data * wp);
            wp_total = wp_total + wp;
//            print(wp_total)
            filtered_image = filtered_image / wp_total;  // 整除，比如得到2.0而不是2，所以后面要转成int型
            Output[img_width*row+col] = (unsigned char)(filtered_image);
        }
    }
    return;
}


/*
gray1相比于gray，把输入的一维数组转成了二维数组，便于后续for循环直接赋值
实际在全志R11平台测试，512*512的图片，gray为42ms，gray1为32ms。效果提升明显。
*/
void bilateral_filter_gray1(unsigned char* Input, unsigned char* Output, unsigned int Width, unsigned int Height, unsigned char diameter, float sigmaColor, float sigmaSpace)
{
    //    不考虑边缘的情况
    float weight_color[256] = {0};
    float weight_space[256];
    float wp_total, gi, gs, wp, filtered_image;
    unsigned char pixel, data;
    int i,j;
    unsigned int img_height, img_width;
    unsigned int end_row, end_col, row, col;
    char radius;
    unsigned char maxk;
    unsigned char** image;

    img_height = Height;
    img_width = Width;


//    计算颜色模板系数表
    for(i=0;i<256;i++)
    {
        weight_color[i] = gaussian((unsigned int)i, sigmaColor);
    }

    image = (unsigned char **)malloc(sizeof(unsigned char *) * img_height); // 分配所有行的首地址
    for (i = 0; (unsigned int)i < img_height; i++)
    { // 按行分配每一列
        image[i] = (unsigned char *)malloc(sizeof(unsigned char) * img_width);
    }
    for(row=0;row<img_height;row++)
    {
        for(col=0; col<img_width; col++)
        {
            image[row][col] = Input[row*img_width+col];
        }
    }

//    计算空间模板
    radius = diameter / 2;
    maxk = 0;
    for(i=-radius;i<radius + 1;i++)
    {
        for(j=-radius;j<radius + 1;j++)
        {
            weight_space[maxk] = gaussian((unsigned int)(i * i + j * j), sigmaSpace);
            maxk = maxk + 1;
        }
    }

    //所有像素进行滤波
    end_row = img_height - diameter / 2;
    end_col = img_width - diameter / 2;
    for(row=(unsigned int)radius;row<end_row;row++)
    {
        for(col=(unsigned int)radius; col<end_col; col++)
        {
            wp_total = 1;
            filtered_image = image[row][col];
            pixel = image[row][col];

//            每个窗口的所有像素
            data = image[row-1][col-1];
            gi = weight_color[abs((int)(data) - (int)(pixel))];
            gs = weight_space[0];// 边界处值域权重为0，空域不做特殊处理
            wp = gi * gs;
            filtered_image = filtered_image + (data * wp);
            wp_total = wp_total + wp;

            data = image[row-1][col];
            gi = weight_color[abs((int)(data) - (int)(pixel))];
            gs = weight_space[1];//边界处值域权重为0，空域不做特殊处理
            wp = gi * gs;
            filtered_image = filtered_image + (data * wp);
            wp_total = wp_total + wp;

            data = image[row-1][col+1];
            gi = weight_color[abs((int)(data) - (int)(pixel))];
            gs = weight_space[2];// 边界处值域权重为0，空域不做特殊处理
            wp = gi * gs;
            filtered_image = filtered_image + (data * wp);
            wp_total = wp_total + wp;

            data = image[row][col-1];
            gi = weight_color[abs((int)(data) - (int)(pixel))];
            gs = weight_space[3];// 边界处值域权重为0，空域不做特殊处理
            wp = gi * gs;
            filtered_image = filtered_image + (data * wp);
            wp_total = wp_total + wp;

//            data = image[row][col];
//            gi = weight_color[abs((int)(data) - (int)(pixel))];
//            gs = weight_space[4];  # 边界处值域权重为0，空域不做特殊处理
//            wp = gi * gs;
//            filtered_image = filtered_image + (data * wp);
//            wp_total = wp_total + wp;

            data = image[row][col+1];
            gi = weight_color[abs((int)(data) - (int)(pixel))];
            gs = weight_space[5]; //边界处值域权重为0，空域不做特殊处理
            wp = gi * gs;
            filtered_image = filtered_image + (data * wp);
            wp_total = wp_total + wp;

            data = image[row+1][col-1];
            gi = weight_color[abs((int)(data) - (int)(pixel))];
            gs = weight_space[6]; //边界处值域权重为0，空域不做特殊处理
            wp = gi * gs;
            filtered_image = filtered_image + (data * wp);
            wp_total = wp_total + wp;

            data = image[row+1][col];
            gi = weight_color[abs((int)(data) - (int)(pixel))];
            gs = weight_space[7]; //边界处值域权重为0，空域不做特殊处理
            wp = gi * gs;
            filtered_image = filtered_image + (data * wp);
            wp_total = wp_total + wp;

            data = image[row+1][col+1];
            gi = weight_color[abs((int)(data) - (int)(pixel))];
            gs = weight_space[8]; //边界处值域权重为0，空域不做特殊处理
            wp = gi * gs;
            filtered_image = filtered_image + (data * wp);
            wp_total = wp_total + wp;
//            print(wp_total)
            filtered_image = filtered_image / wp_total;  // 整除，比如得到2.0而不是2，所以后面要转成int型
            Output[img_width*row+col] = (unsigned char)(filtered_image);
        }
    }

    for (int i = 0; (unsigned int)i < img_height; ++i)
    {
        free(*(image + i));
    }
    free(image);
    return;
}



//计算亮度中值和灰度<=中值的像素点个数
unsigned int calMediaValue(const unsigned int histogram[], unsigned int thresh)
{
    unsigned int sum = 0;
    for (unsigned int i = 0; i < 256; i++)
    {
        sum += histogram[i];
        if (sum>= thresh)
        {
            return i;
        }
    }
    return 255;
}


/*
以下函数是对统计直方图计算方法的一次改进，
实际上图像中绝大部分的信息是相同的，没必要每次都从0-255进行遍历
j = i-5; j < i+5; j++ 这部分进行赋值限制，实际如果图像噪声比较小，可以减小该阈值，
噪声比较大，可以增加该阈值。
*/
unsigned int calMediaValue1(const unsigned int histogram[], unsigned int thresh)
{
    unsigned int sum = 0;
    static unsigned int i=0;
    unsigned int j;
    if(i<10)
    {
        for (i = 0; i < 256; i++)
        {
            sum += histogram[i];
            if (sum>= thresh)
            {
                return i;
            }
        }
    }
    else if(i>240)
    {
        for (i = 230; i < 256; i++)
        {
            sum += histogram[i];
            if (sum>= thresh)
            {
                return i;
            }
        }
        sum = 0;
        for (i = 0; i < 256; i++)
        {
            sum += histogram[i];
            if (sum>= thresh)
            {
                return i;
            }
        }
    }
    else
    {
        for (j = i-5; j < i+5; j++)
        {
            sum += histogram[j];
            if (sum>= thresh)
            {
                return j;
            }
        }
        sum = 0;
        for (i = 0; i < 256; i++)
        {
            sum += histogram[i];
            if (sum>= thresh)
            {
                return i;
            }
        }
    }

    return 255;
}



/*
在PC机上测试512*512的图像，基于直方图的中值滤波为80ms左右，跟矩阵的直径没有太大关系。
3*3矩阵时，利用排序法测得是最快的，58ms左右，当矩阵增大时，效果就远不如基于直方图了。

在全志R11平台测试，选择3*3矩阵时，插入排序法为156ms，直方图均值滤波为256ms，所以
基于直方图的均值滤波还有待优化。
*/
void fastMedianBlur_gray(const unsigned char* srcImg, unsigned char* dstImg, unsigned int Width, unsigned int Height, unsigned int diameter)
{

    unsigned int imgW = Width;
    unsigned int imgH = Height;

    //直方图
    unsigned int windowSize = diameter*diameter;
    unsigned int radius = (diameter - 1) / 2;
    unsigned int Hist[256]={0};
    unsigned int thresholdValue = windowSize / 2 + 1;
    unsigned int end_row = imgH-radius;
    unsigned int end_col = imgW-radius;


    for (unsigned int row=radius; row<end_row; row++)
    {
        for (unsigned int col=radius; col<end_col; col++)
        {
            //每一行第一个待滤波像素建立直方图
            if(col==radius)
            {
                memset(Hist, 0, (sizeof(Hist)/sizeof (Hist[0]))*sizeof(int));
                for (unsigned int y=row-radius; y<=row+radius; y++)
                {
                    for (unsigned int x=col-radius; x<=col+radius; x++)
                    {
                        unsigned char value=srcImg[ y*imgW+x];
                        Hist[value]++;
                    }
                }
            }
            else//更新直方图
            {
                unsigned int left=col-radius-1;
                unsigned int right=col+radius;
                for (unsigned int y=row-radius; y<=row+radius; y++)
                {
                    //减去左边一列
                    unsigned int leftIdx=y*imgW+left;
                    unsigned char leftValue=srcImg[leftIdx];
                    Hist[leftValue]--;

                    //加上右边一列
                    unsigned int rightIdx=y*imgW+right;
                    unsigned char rightValue=srcImg[rightIdx];
                    Hist[rightValue]++;
                }
            }

            //直方图求中值
//            unsigned char filterValue=(unsigned char)calMediaValue(Hist, thresholdValue);
            unsigned char filterValue=(unsigned char)calMediaValue1(Hist, thresholdValue);
            dstImg[row*imgW+col]=filterValue;
        }
    }

    //边界直接赋原始值，不做滤波处理 如果不考虑边界情况，下面内容甚至可不比执行
    //上下边界 两者选其一，效率待测试
//    memcpy(dstImg, srcImg, radius*imgW);
//    memcpy(&dstImg[(imgH - radius-1)*imgW], &srcImg[(imgH - radius-1)*imgW], radius*imgW);
    for (unsigned int i = 0; i < radius; i++)
    {
        for (unsigned int j = 0; j < imgW; j++)
        {
            unsigned int idxTop = i*imgW + j;
            dstImg[idxTop] = srcImg[idxTop];
            unsigned int idxBot = (imgH - i - 1)*imgW + j;
            dstImg[idxBot] = srcImg[idxBot];
        }
    }

    //左右边界
    for (unsigned int i = 0; i < radius; i++)
    {
        for (unsigned int j = radius; j < imgH - radius - 1; j++)
        {
            unsigned int idxLeft = j*imgW + i;
            dstImg[idxLeft] = srcImg[idxLeft];
            unsigned int idxRight = j*imgW + imgW - radius + i;
            dstImg[idxRight] = srcImg[idxRight];
        }
    }
}


void normalMedianBlur_gray(const unsigned char* srcImg, unsigned char* dstImg, unsigned int Width, unsigned int Height, unsigned int diameter, char* mode)
{

    unsigned int imgW = Width;
    unsigned int imgH = Height;

    //直方图
    unsigned int windowSize = diameter*diameter;
    unsigned int radius = (diameter - 1) / 2;
    unsigned int end_row = imgH-radius;
    unsigned int end_col = imgW-radius;

    //排序
    int midarr[256];
//    int midarr[windowSize];

    for (unsigned int row=radius; row<end_row; row++)
    {
        for (unsigned int col=radius; col<end_col; col++)
        {
            int mark = 0;
            for (unsigned int y=row-radius; y<=row+radius; y++)
            {
                for (unsigned int x=col-radius; x<=col+radius; x++)
                {
                    midarr[mark++]=srcImg[ y*imgW+x];
                }
            }
            cj_sort(midarr, (int)windowSize, 0, (int)windowSize, mode);

            //直方图求中值
            unsigned char filterValue=(unsigned char)midarr[windowSize/2];
            dstImg[row*imgW+col]=filterValue;
        }
    }

    //边界直接赋原始值，不做滤波处理 如果不考虑边界情况，下面内容甚至可不比执行
    //上下边界 两者选其一，效率待测试
//    memcpy(dstImg, srcImg, radius*imgW);
//    memcpy(&dstImg[(imgH - radius-1)*imgW], &srcImg[(imgH - radius-1)*imgW], radius*imgW);
    for (unsigned int i = 0; i < radius; i++)
    {
        for (unsigned int j = 0; j < imgW; j++)
        {
            unsigned int idxTop = i*imgW + j;
            dstImg[idxTop] = srcImg[idxTop];
            unsigned int idxBot = (imgH - i - 1)*imgW + j;
            dstImg[idxBot] = srcImg[idxBot];
        }
    }

    //左右边界
    for (unsigned int i = 0; i < radius; i++)
    {
        for (unsigned int j = radius; j < imgH - radius - 1; j++)
        {
            unsigned int idxLeft = j*imgW + i;
            dstImg[idxLeft] = srcImg[idxLeft];
            unsigned int idxRight = j*imgW + imgW - radius + i;
            dstImg[idxRight] = srcImg[idxRight];
        }
    }
}


unsigned char Clamp2Byte(int n)
{
    return (((255 - n) >> 31) | (n & ~(n >> 31)));
}

void Convolution2D(unsigned char * data, unsigned char* dstImg, unsigned int width, unsigned int height,
                   unsigned int channels, int * filter, unsigned char filterW, unsigned char cfactor, unsigned char bias)
{
    unsigned char * tmpData = (unsigned char * ) malloc(width * height * channels);
    int factor = 256 / cfactor;
    unsigned char halfW = filterW / 2;
    if (channels == 3 || channels == 4)
    {
        for (unsigned int y = 0; y < height; y++)
        {
            unsigned int y1 = y + height - halfW;
            for (unsigned int x = 0; x < width; x++)
            {
                unsigned int x1 = x + width - halfW;
                int r = 0;
                int g = 0;
                int b = 0;
                unsigned int p = (y * width + x) * channels;
                for (unsigned int fx = 0; fx < filterW; fx++)
                {
                    unsigned int dx = (x1 + fx) % width;
                    unsigned int fidx = fx * (filterW);
                    for (unsigned int fy = 0; fy < filterW; fy++)
                    {
                        unsigned int pos = (((y1 + fy) % height) * width + dx) * channels;
                        int * pfilter = & filter[fidx + (fy)];
                        r += data[pos] * ( * pfilter);
                        g += data[pos + 1] * ( * pfilter);
                        b += data[pos + 2] * ( * pfilter);
                    }
                }
                tmpData[p] = Clamp2Byte(((factor * r) >> 8) + bias);
                tmpData[p + 1] = Clamp2Byte(((factor * g) >> 8) + bias);
                tmpData[p + 2] = Clamp2Byte(((factor * b) >> 8) + bias);
            }
        }
    }
    else if (channels == 1)
    {
        for (unsigned int y = 0; y < height; y++)
        {
            unsigned int y1 = y - halfW + height;
            for (unsigned int x = 0; x < width; x++)
            {
                int r = 0;
                unsigned int x1 = x - halfW + width;
                /*
                在pc机上测试640*480的图像，for循环1为40-45ms，for循环2为32-35ms，相差很大。
                在全志R11上测试，两者都为645ms，无差别。
                */
//                for (unsigned int fy = 0; fy < filterW; fy++)
//                {//for循环1
//                    unsigned int dy = (y1 + fy) % height;
//                    for (unsigned int fx = 0; fx < filterW; fx++)
//                    {
//                        unsigned int dx = (x1 + fx) % width;
//                        unsigned int pos = (dy * width + dx);
//                        int szfilter = filter[fx * filterW + fy];
//                        r += data[pos] * szfilter;
//                    }
//                }
                for (unsigned int fy = 0; fy < filterW; fy++)
                {//for循环2
                    unsigned int dy = (y1 + fy) % height;
                    for (unsigned int fx = 0; fx < filterW; fx++)
                    {
                        r += data[dy * width + (x1 + fx) % width] * filter[fx * filterW + fy];
                    }
                }

//                tmpData[y * width + x] = Clamp2Byte(((factor * r) >> 8) + bias);
                tmpData[y * width + x] = r;
            }
        }
    }
    memcpy(dstImg, tmpData, width * height * channels);

    free(tmpData);
}
