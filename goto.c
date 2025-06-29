#include<stdio.h>
int main()
{
	printf("Hello World\n");
	int i=2;
	goto L1;
	for(i=0;i<5;i++)
	{
L1:printf("%d",i);
	}
}
