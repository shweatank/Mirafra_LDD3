#include<stdio.h>
#define largest(a,b,c) (((a>b)&&(a>c))?(a>c)?a:c:(b>c)?b:c)
void main()
{
	int a,b,c;
	printf("Enter the 3 numbers : ");
	scanf("%d%d%d",&a,&b,&c);
	int result=largest(a,b,c);
	printf("Largest = %d\n",result);
}
