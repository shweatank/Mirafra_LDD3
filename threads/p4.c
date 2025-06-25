#include<stdio.h>
#include<unistd.h>
int main()
{
	int p[2];
	pipe(p);
		int op;
		int x=20,y=10,result;
	if(fork)
	{
		printf("Enter the option:1)add 2)sub 3)mul 4)div\n");
		scanf("%d",&op);
		switch(op)
		{
			case 1:result=x+y;
			       break;
			case 2:result=x-y; break;
			case 3:result=x*y; break;
			case 4:result=x/y; break;
			default:printf("enterd option is invalid\n");
		}
		close(p[0]);
		write(p[1],&result,sizeof(result));
	}
	else
	{
	close(p[1]);
        read(p[0],&result,sizeof(result));
	printf("Result from child:%d",result);
	}
}
