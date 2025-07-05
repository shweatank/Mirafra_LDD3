#include<stdio.h>
int main()
{
        int num,i=0,digit;
        char arr[10];
        printf("enter the num\n");
        scanf("%d",&num);
        while(num)
        {
                digit=num%8;
                arr[i]=digit+48;
                num=num/8;
                i++;
        }
        for(--i;i>=0;i--)
        {
                printf("%c",arr[i]);
        }
}

