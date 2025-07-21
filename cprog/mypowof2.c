#include<stdio.h>

int mypowofn(int x,int n)
{
	int pow=1;
	for(int i=0;i<n;i++)
	{
		pow=pow*x;
	}
	return pow;
}
int mypowofn1(int x,int n)
{
	if(n==1)
	{
		return x;
	}
	else
		return x*(mypowofn(x,n-1));
}

int main()
{
	int x,n;
	scanf("%d %d",&x,&n);
	int pow=mypowofn1(x,n);
	printf("pow of x^n =%d\n",pow);
	return 0;
}
