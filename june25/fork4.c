#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
int add(int a, int b)
{
	return a+b;
}
int sub(int a,int b)
{
	return a-b;
}
int mul(int a, int b)
{
	return a*b;
}
int divi(int a, int b)
{
	return a/b;
}
void main()
{
	int ret,result;
	int p[2];
	pipe(p);
	perror("pipe");
	ret=fork();
	perror("fork");
	if(ret>0)
	{
		close(p[0]);
		int op,a,b;
		printf("Enter option 1-add , 2-sub , 3-mul , 4-div\n");
		scanf("%d",&op);
		printf("Enter the operands : ");
		scanf("%d%d",&a,&b);
		switch(op)
		{
			case 1:result=add(a,b);
			       break;
			case 2:result=sub(a,b);
			       break;
			case 3:result=mul(a,b);
			       break;
			case 4:result=divi(a,b);
			       break;
			default:printf("Invalid choice\n");
		}
		write(p[1],&result,4);
		exit(0);
	}
	else if(ret==0)
	{
		close(p[1]);
		wait(0);
		read(p[0],&result,4);
		printf("In child result = %d\n",result);
	}
}
