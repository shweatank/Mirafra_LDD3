#include<stdio.h>
#include<unistd.h>
int main()
{
	int x=0;
	//int *p=&x;
	if(fork()==0)
	{
		x+=30;
		int *p=&x;
		printf("x=%d addr=%ld\n",x,p);
	}
	else
	{
		x+=20;
		int *p=&x;
		printf("x=%d  addr=%ld\n",x,p);
	}
}
