#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>

int global_var = 10;

int main()
{
//	int global_var = 10;
	int pid;
	printf("BEFORE FORK value:%d\n",global_var);
	printf("ADDRESS %p\n",&global_var);

	pid = fork();

	if(pid<0)
	{
		perror("FORK FAILED");
		return 1;
	}

	else if(pid == 0)
	{
		global_var +=5;
		printf("CHILD PROCESS value %d\n",global_var);
		printf("C_ADDRESS %p\n",&global_var);
	}
	else
	{
		global_var -=5;
		printf("PARENT PROCESS value %d\n",global_var);
		printf("P_ADDRESS %p\n",&global_var);
	}
	return 0;
}
