#include <stdio.h>

static void my_func() 
{
    printf("Static function called!\n");
}

void (*fp())() 
{
    return my_func;
}

