// main.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc ,char *argv[]) 
{

   if(argc == 4)
   {
	   double a =atof(argv[1]);
	   char op =argv[2][0];
	   double b =atof(argv[3]);
	   double result;

	   switch(op)
	   {
		case '+' : result =a+b;break;
		case '-' : result =a-b;break;
		case '*' : result =a*b;break;
		case '/' : result =a/b;break;
		default : printf("UNsupported OP\n");
	        return 1;
	   }
	 printf("Result :%f\n",result);
	 return 0;
   }
   
	
    char num1[16], num2[16], op[4];

    // Get input from user
    printf("Enter first number: ");
    scanf("%15s", num1);
    printf("Enter operator (+, -, *, /): ");
    scanf("%3s", op);
    printf("Enter second number: ");
    scanf("%15s", num2);

    pid_t pid = fork();

    if (pid == 0) {
        // Child process
        execl("./fork_calc", "fork_calc", num1, op, num2, (char *)NULL);
        perror("execl failed"); // if exec fails
        exit(1);
    } else if (pid > 0) {
        // Parent process
        wait(NULL); // Wait for child to finish
        printf("Parent: Calculation done by child.\n");
    } else {
        perror("fork failed");
        return 1;
    }

    return 0;
}

