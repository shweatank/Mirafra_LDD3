#include<stdio.h>
int multiply(int a,int b)
{
	int res=0;
	while(b>0)
	{
		if(b&1)
			res+=a;
		a<<=1;
		b>>=1;
	}
	return res;
}
int main()
{
	int num1,num2;
	printf("enter 2 numbers:\n");
	scanf("%d%d",&num1,&num2);
	int res=multiply(num1,num2);
	printf("res:%d\n",res);
}

