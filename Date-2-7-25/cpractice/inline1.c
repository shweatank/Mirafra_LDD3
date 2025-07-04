#include<stdio.h>
int add(int a,int b)
{
	return a+b;
}

int main()
{
	int z=add(10,20);
	printf("z=%d\n",z);
	
	z=add(20,30);
	printf("z=%d\n",z);
	
	z=add(30,40);
	printf("z=%d\n",z);
}
