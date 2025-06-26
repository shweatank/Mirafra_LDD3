#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int caluc(int a,int b,int op)
{
	int c=0;
	switch(op)
	{
		case 1:
			c=a+b;
			break;
		case 2:
			c=a-b;
			break;
		case 3:
			c=a*b;
			break;
		case 4:
			c=a/b;
			break;
		default:
			break;
	}
	return c;
}

int main()
{
	int a,b,c,p[2];
	pipe(p);
	printf("enter a and b: ");
	scanf("%d",&a);
	scanf("%d",&b);
	if(fork()==0)
	{
		read(p[0],&c,sizeof(int));
		printf("result : %d \n",c);
	}
	else
	{
		int op;
		printf("enter the op to be performed 1.add 2.sub 3.mul 4.div");
		scanf("%d",&op);
		c=caluc(a,b,op);
		write(p[1],&c,sizeof(int));
	}

}
