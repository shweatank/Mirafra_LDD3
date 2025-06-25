#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

void calculator(const char *op, int a, int b) {
    int result = 0;

    if (strcmp(op, "add") == 0) 
    {
        result = a + b;
    } 
    else if (strcmp(op, "sub") == 0) 
    {
        result = a - b;
    } 
    else if (strcmp(op, "mul") == 0) 
    {
        result = a * b;
    }
    else if (strcmp(op, "div") == 0) 
    {
        if (b == 0) 
	{
            fprintf(stderr, "Error: Division by zero\n");
            exit(1);
        }
        result = a / b;
    } 
    else 
    {
        fprintf(stderr, "Unknown operation: %s\n", op);
        exit(1);
    }

    printf("Child: Result of %s(%d, %d) = %d\n", op, a, b, result);
}

int main() 
{
    char op[10];
    int a, b;

    printf("Enter operation (add/sub/mul/div): ");
    scanf("%s", op);

    printf("Enter two integers: ");
    scanf("%d %d", &a, &b);

    pid_t pid = fork();

    if (pid < 0) 
    {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } 
    else if (pid == 0) 
    {
        calculator(op, a, b);
    } 
    else 
    {
        wait(NULL);
        printf("Parent: Child process completed.\n");
    }

    return 0;
}

