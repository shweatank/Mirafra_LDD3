#include<stdio.h>
int check()
{
	printf("Hello\n");
	return 1;
}

int main()
{
	int x=0;
	if(x && check())
	{
		printf("Hi\n");
	}
	else
	{
		printf("Bye\n");
	}
	return 0;
}
