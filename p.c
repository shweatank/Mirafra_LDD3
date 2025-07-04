#include<stdio.h>
int main()
{
    volatile  int i=1;
    printf("%d\n",i++ + ++i);
}
