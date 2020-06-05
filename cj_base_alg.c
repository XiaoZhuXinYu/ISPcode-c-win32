#include<stdlib.h>
#include<string.h>
#include "include/cj_base_alg.h"
#include "include/cj_debug.h"


/*
n种排序方法，都是从大到小排列
算法是直接参考了博客 https://blog.csdn.net/hiudawn/article/details/80380572
并没有验证其准确性，只是解决了一些编译器警告问题。
参考这些算法的目的也是为了和快速中值滤波的效率做一个比较。
*/

//辅助函数：交换两个变量
void swap(int*a,int*p)
{
    int temp = *a;
    *a = *p;
    *p = temp;
}


//冒泡排序
//两两对比，把最大的一直挪到最后面
//非常不怎样的一个算法，但确是大部分高校教的第一个算法
void bubbleSort(int* arr,int len)
{
    int i,j;
    //这个是上界
    for(i=0;i<len;i++)
    {
        //一般导数第i+1个以后都已经排好了
        for(j=1;j<len-i;j++)
        {
            //大的一直冒泡到最后面去
            if(arr[j-1]>arr[j])
            {
                swap(&arr[j-1],&arr[j]);
            }
        }
    }
}

//选择排序
//把最小的一个个放到第一第二第三个
//已放好的叫有序区，没放好的是无序区，有序区一旦放好就不会变了
void selectSort(int* arr,int len)
{
    //i大循环，j大小对比
    int i,j;
    int temp,index;//临时对照变量
    for(i=0;i<len;i++)
    {
        //先假设无序区第一个是最小
        temp = arr[i];
        index = i;
        for(j=i+1;j<len;j++)
        {
            //找到无序区真正最小的
            if(arr[j]<temp)
            {
                temp = arr[j];
                index = j;
            }
        }
        //如果第一个就是最小的，就不用进行什么交换
        if(index==i)
        continue;
        //不然就交换无序区第一个数和无序区最小的数
        swap(&arr[i],&arr[index]);
    }
}

//插入排序
//从第1个数开始，往后开始遍历，第i个数一定要放到使得前i个数都变成有序的。
void insertSort(int* arr,int len)
{
    int i,j;
    for(i=0;i<len;i++)
    {
        for(j=i;j>0;j--)
        {
            if(arr[j]<arr[j-1])
            {
                swap(&arr[j],&arr[j-1]);
            }
            else
            {
                //已经不比那个元素小了就提前退出
                break;
            }
        }
    }
}

//希尔排序
//插入排序的加强版，不是一次性进行插入，而是分成一拨拨来进行
//比如奇数下标的为一拨，偶数下标的为一拨，然后再对分好的两拨进行插入排序
//也就是一开始是隔一定step>1进行插入排序，最后的step=1
//这个步长的变动方式有多种，可是是 step:=step/3+1
//对大量的数据，排序效率明显比插入排序高
void shellSort(int* arr,int len)
{
    int step = len;
    //do while比较好，保证step为1还能再排一次
    do
    {
        //这句一定要放这里，不然步长为1就跳出去了，最后一次无法排序
        step = step/3 +1;
        int i,j,k;
        //分拨排序,一共有step拨
        for(i=0;i<step;i++)
        {
            for(j=i;j<len;j+=step)
            {
                for(k=j;k>i;k-=step)
                {
                    if(arr[k]<arr[k-step])
                    {
                        swap(&arr[k],&arr[k-step]);
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
    }while(step>1);

}

//快速排序
//顾名思义，排得真的很快。用递归思想
//每次找一个基准数，以其为参考点，比它小的放左边，大的放右边（这两堆内部可能是无序的）
//再把分好的两堆各自找个基准数，按前面的步骤再来，直至数据不能再分，排序完毕
//基准先挖出来，有i，j两个指针，一开始j往左挪，如果遇到比基准小的，填到基准位置
//之后换i往后挪，遇到比基准大的，就放到j的那个坑。全部跑完后，基准丢到最后剩出来的那个坑。
void quickSort(int* arr,int start,int end)
{
    //递归最重要的就是设置退出条件，如下
    if(start>=end)
    {
        return;
    }
    int i = start;
    int j = end;
    int temp = arr[i];
    //如果右指针j一直没小于左指针i，一直跑
    while(i<j)
    {
        //先从右边找比基准小的，找到和基准交换，但要保留j值
        while(i<j)
        {
            if(arr[j]<temp)
            {
                swap(&arr[j],&arr[i]);
                break;
            }
            j--;
        }

        //右边找到一个比基准小的之后，轮到左边找比基准大的，然后和上面空出的j位置交换
        while(i<j)
        {
            if(arr[i]>temp)
            {
                swap(&arr[j],&arr[i]);
                break;
            }
            i++;
        }

    }
    //排左半区
    quickSort(arr,start,i-1);
    //排右半区
    quickSort(arr,i+1,end);
}

//归并排序
//本质上是把两个已经排好的序列合并成一个
//如果对一个随机序列的两两元素来看，那么每个元素都是排好的序列
//可以把一个数组拆分成前后两半来做这件事
//这个算法需要额外的辅助空间，用来存放归并好的结果
void mergeSort(int* arr,int start,int end)
{
    if(start>=end)
    {
        return;
    }
    int i = start;
    int mid = (start+end)/2;
    int j = mid + 1;
    mergeSort(arr,i,mid);
    mergeSort(arr,j,end);

    //合并
    //其实我觉得不用这个额外的空间也行，两个子序列再排一次能减少空间，不过速度肯定会有影响
    int* temp = (int*)malloc((unsigned int)(end-start+1)*sizeof(int));
    int index = 0;
    //开始对比两个子序列，头部最小的那个数放到新空间
    while(i<=mid&&j<=end)
    {
        if(arr[i]<=arr[j])
        {
            temp[index++] = arr[i++];
        }else
        {
            temp[index++] = arr[j++];
        }
    }
    //总有一个序列是还没有放完的，这里再遍历一下没放完的
    while(i<=mid)
    {
        temp[index++] = arr[i++];
    }
    while(j<=end)
    {
        temp[index++] = arr[j++];
    }
    //排完再把新空间的元素放回旧空间
    int k;
    for(k= start;k<=end;k++)
    {
        //哎，temp的下标写错，排查了一个钟，真菜
        arr[k] = temp[k-start];
    }
    free(temp);
}

//堆排序
/*
这个堆是数据结构堆，不是内存malloc相关的那个堆--我曾经理解错n久
根节点比孩子节点都大的叫大顶堆（包括子树的根），比孩子节点小的叫小顶堆
从小到大排序用的是大顶堆，所以要先构造这种堆，然后把这个根的最大元素交换到最尾巴去
每次拿走一个最大元素，待排序的队列就慢慢变短
主要步骤1，初始化构造这种大顶堆，把堆顶最大的数放到最尾巴，数列长度减少1，再次构建大顶堆
2,这时候只有堆顶元素不满足大顶堆，那么其实只要从堆顶元素开始慢慢微调而已，没必要再完全重新建堆，想要也可以，不过很浪费时间
理解起来确实很难，涉及到完全二叉树
孩子节点i的爸爸是i/2，爸爸节点的儿子是2i和2i+1。
第一次初始化之后充分利用子树已经是大顶堆
*/
void adjust(int* arr,int len,int index)
{
    //调整函数，把孩子、父亲中的最大值放到父亲节点
    //index为待调整节点下标,一开始设它最大
    int max = index;
    int left = 2*index+1;//左孩子
    int right = 2*index+2;//右孩子
    if(left<len && arr[left] > arr[max])
    {
        max = left;
    }
    if(right<len && arr[right] > arr[max])
    {
        max = right;
    }
    //如果父亲节点不是最大
    if(max!=index)
    {
        //一旦上层节点影响了某个孩子节点，还要观察以这个孩子节点为父节点的子树是不是也不是大顶堆了
        swap(&arr[index],&arr[max]);
        //因为发生了交换，还要继续调整受到影响的孩子节点
        //***************************************
        adjust(arr,len,max);//这句话非常非常关键
        //***************************************
        /*
            只有父亲和孩子节点发生了交换，才有继续调整孩子的必要，如果无脑在不是这里面递归，堆排序的效果不会比冒泡好到哪去
            而如果写在了这里面，虽然还是pk不过快排，但好歹和快排的差距只缩小到个位数倍数的量级（小数据量的时候）
            堆排序一个优点是空间复杂度也不高
        */
    }
}
//主要排序部分
void heapSort(int* arr,int len)
{
    //初始化大顶堆
    //initHeap(arr,i,0);
    //从最后一个非叶子节点开始
    //第一次一定要从下至上一直排，一开始是乱序的
    int i;
    for(i=len/2-1;i>=0;i--)
    {
        adjust(arr,len,i);
    }
    swap(&arr[0],&arr[len-1]);


    //第二次之后，只需要从根节点从上到下调整，遇到没发生交换的直接可以退出循环了
    //微调得到大顶堆（因为只有堆顶不满足而已）
    int j; //去掉尾节点后的数组长度
    //把最大值交换到最后
    for(j=len -1;j>0;j--)
    {
        adjust(arr,j,0);
        swap(&arr[0],&arr[j-1]);
    }
}


//对外接口函数
void cj_sort(int* arr,int len, int start, int end, char* mode)
{
    if(strcmp(mode,"bubble")==0)
    {
        //冒泡排序
        bubbleSort(arr, len);
    }
    else if(strcmp(mode,"select")==0)
    {
//        选择排序
        selectSort(arr, len);
    }
    else if(strcmp(mode,"insert")==0)
    {
        //插入排序
        insertSort(arr, len);
    }
    else if(strcmp(mode,"shell")==0)
    {
//       希尔排序
        shellSort(arr, len);
    }
    else if(strcmp(mode,"quick")==0)
    {
//        快速排序
        quickSort(arr, start, end);
    }
    else if(strcmp(mode,"merge")==0)
    {
//         归并排序
        mergeSort(arr, start, end);
    }
    else if(strcmp(mode,"heap")==0)
    {
//        堆排序
        heapSort(arr, len);
    }
}

