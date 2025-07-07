#include<stdio.h>
#define largest(a,b,c) ((a>b)?((a>c)?a:c):((b>c)?b:c))
int main()
{
	int a,b,c;
	printf("enter 3 numbers:\n");
	scanf("%d%d%d",&a,&b,&c);
	int res=largest(a,b,c);
	printf("largest:%d\n",res);
}
