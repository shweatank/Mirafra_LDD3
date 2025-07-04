#include<stdio.h>
int main()
{
        int num=1;
        char *ptr=(char *)&num;
        if(*ptr==1)
                printf("little endian\n");
        else
                printf("big endian\n");
}
