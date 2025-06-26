#include<stdio.h>
#include<unistd.h>
int b=10;
int main()
{

        int a=10;
	if(fork()==0)
	{
	printf("child a=%d b=%d a=%p b=%p\n",a,b,&a,&b);
	}
	else
	{
	printf("parent a=%d b=%d a=%p b=%p\n",a,b,&a,&b);
	}
}

