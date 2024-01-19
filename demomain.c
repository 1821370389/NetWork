#include<stdio.h>


int main()
{
    /* */
    int num = 121;              // 10进制
    printf("num:0x%x\n",num);   // 79
    num = 0x121;                // 16进制
    printf("num:0x%x\n",num);   // 121
    num = 0121;                 // 8进制
    printf("num:0x%x\n",num);   // 81   

    return 0;
}