#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int x = 10;

int main()
{
	//int x =10;
    pid_t pid = fork();

    if (pid == 0)
    {
        x += 5;
        printf("child process = %d  address:%p\n", x,(void*)&x);
    } else
    {
        x -= 3;
        printf("parent process = %d address:%p", x, (void*)&x);
    }

    return 0;
}

