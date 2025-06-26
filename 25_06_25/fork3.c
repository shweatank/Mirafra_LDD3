#include<unistd.h>
#include<sys/wait.h>
#include<stdio.h>
int add(int a,int b)
{
	return a+b;
}
int sub(int a,int b)
{
	return a-b;
}
int divi(int a,int b)
{
	return a/b;
}
int mul(int a,int b)
{
	return a*b;
}
int main()
{
	int x=10,y=5,ch,ret=30;
	pid_t q=fork();
	if(q<0)
	{
		perror("fork");
		return -1;
	}
	else if(q==0)
	{
		printf("1.Add\n");
		printf("2.Sub\n");
		printf("3.Divi\n");
		printf("4.Mul\n");
		printf("Enter your choice ");
		scanf("%d",&ch);
		switch(ch)
		{
			case 1: ret=add(x,y);
				break;
			case 2: ret=sub(x,y);
				break;
			case 3: ret=divi(x,y);
				break;
			case 4: ret=mul(x,y);
				break;
			default: printf("Invalid input\n");
		}
		printf("%d\n",ret);
	}
	else
	{
		wait(NULL);
		printf("Result: %d\n",ret);
	}
	return 0;
}
