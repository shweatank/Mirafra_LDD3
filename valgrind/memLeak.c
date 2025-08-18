#include<stdio.h>
#include<stdlib.h>
#include<string.h>


void create_leak()
{
    char *ptr1= malloc(50); //Allocate 50 bytes
    strcpy(ptr1, "This is a memory leak example.");
    
    char *ptr2= malloc(100); //Allocate 100 bytes
    strcpy(ptr1, "This is a memory leak example.");

    free(ptr1);
}

int main()
{
    printf("running...\n");
    create_leak();
    printf("Done.\n");
    return 0;
}
