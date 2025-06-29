#include <stdio.h>

void add(int a,int b) 
{ 
	printf("Add:%d\n",a+b); 
}
void subtract(int a,int b) 
{ 
	printf("Subtract:%d\n",a-b); 
}
void multiply(int a,int b) 
{ 
	printf("Multiply:%d\n",a*b); 
}

int main() 
{
    void (*operations[])(int,int) = {add, subtract, multiply};

    int choice;
    printf("Enter 1 for add, 2 for subtract, 3 for multiply: ");
    scanf("%d", &choice);

    if (choice >= 1 && choice <= 3)
        operations[choice-1](3,2);
    else
        printf("Invalid choice\n");

    return 0;
}

