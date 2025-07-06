#include<stdio.h>
int fibo(int num)
{
	if(num==0)
		return 0;
	else if(num==1)
		return 1;
	else
		return fibo(num-1)+fibo(num-2);
}
void main()
{
	int num;
	printf("Enter the number : ");
	scanf("%d",&num);
	printf("%dth term = %d\n",num,fibo(num));
}
