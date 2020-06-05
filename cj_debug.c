#include "include/cj_debug.h"

long cj_gettime(char* mode)
{
    struct timeval time_now = {0};
    long time_sec = 0;//秒
    long time_mil = 0;//1毫秒 = 1秒/1000
    long time_mic = 0;//1微秒 = 1毫秒/1000

    gettimeofday(&time_now,NULL);
    time_sec = time_now.tv_sec;
    time_mil = time_sec * 1000 + time_now.tv_usec/1000;
    time_mic = time_now.tv_sec*1000*1000 + time_now.tv_usec;
    if(strcmp(mode,"s")==0)
    {
//        printf("s: %ld\n",time_sec);
        return time_sec;
    }
    else if(strcmp(mode,"ms")==0)
    {
//        printf("ms: %ld\n",time_mil);
        return time_mil;
    }
    else if(strcmp(mode,"us")==0)
    {
        printf("us: %ld\n",time_mic);
        return time_mic;
    }

    return 0;
}


//打印数组
void cj_printArr(int* arr,int len)
{
    int i;
    for(i=0;i<len;i++)
    {
        printf("%d\t",*(arr++));
    }
    printf("\n");
}

