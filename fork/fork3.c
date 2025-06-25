#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main ()
{
     pid_t pid;
     pid = fork();
     if (pid<0)
     {
	     fprintf(stderr, "E: fork failed.\n");
	     return 1;
     }
     else if (pid ==0 )
     {
	execlp ("/bin/ls", "ls", NULL);

     }
     else 
     {
	     wait (NULL);
	     printf ("child completed");
     }
     return 0;

}
