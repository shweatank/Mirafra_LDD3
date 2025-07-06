#include<stdio.h>
int decimal(int num,int start,int stop)
{
	int dec=0;
	for( ; stop>=start;stop--)
		dec=dec*2+((num>>stop)&1);
	return dec;
}
void main()
{
	int num,start,stop,res;
	printf("Enter the number,start and stop bits :");
	scanf("%d%d%d",&num,&start,&stop);
	res=decimal(num,start,stop);
	printf("Result = %d\n",res);
}
