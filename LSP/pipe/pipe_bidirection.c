#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
int main()
{
	pid_t pid;
	int fd1[2],fd2[2];
	char p_msg[]="hello from parent";
	char c_msg[]="hello from child";
	char buf[50];
	if((pipe(fd1)==-1)||(pipe(fd2)==-1))
	{
		perror("pipe");
		return EXIT_FAILURE;
	}
	pid=fork();
	if(pid==-1)
	{
		perror("fork");
		return EXIT_FAILURE;
	}
	else if(pid>0)
	{
		close(fd1[0]);
		close(fd2[1]);
		int len1=strlen(p_msg)+1;
		if(write(fd1[1],p_msg,len1)==len1)
			printf("parent successfully written data \n");
		else
			printf("parent failed to write\n");
		if(read(fd2[0],buf,sizeof(buf))>0)
			printf("parent recieved:%s\n",buf);
	}
	else
	{
		close(fd1[1]);
		close(fd2[0]);
		int len2=strlen(c_msg)+1;
		if(read(fd1[0],buf,sizeof(buf))>0)
			printf("child recieved:%s\n",buf);
		if(write(fd2[1],c_msg,len2)==len2)
			printf("child successfully written data \n");
		else
			printf("child failed to write\n");
	}
}


/*Case you asked: "Parent executes first"

Parent runs first after fork().
It closes unused ends.
Writes "hello from parent" into fd1[1]. ✅
Then it tries to read(fd2[0], ...).
At this moment, the parent’s read() will block because:
Nothing has been written yet into fd2[1] (child’s responsibility).

Pipes are blocking by default → if there’s no data, read() waits.

Then the child process gets scheduled:
It reads "hello from parent" from fd1[0]. ✅
It writes "hello from child" into fd2[1]. ✅
Now the parent’s read(fd2[0]) unblocks and receives the message. ✅*/
