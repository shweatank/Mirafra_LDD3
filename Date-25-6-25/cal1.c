#include<stdio.h>
#include<unistd.h>
int add(int a,int b)
{
	return a+b;
}
int main()
{
	int fd[2];
	pipe(fd);
	if(fork()==0)
	{
		printf("child \n");
		int z=add(10,20);
		write(fd[1],&z,sizeof(z));
	}
	else
	{
		int res;
		read(fd[0],&res,sizeof(res));
		printf("parent res=%d\n",res);
	}
}

