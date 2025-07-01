#include <stdio.h>


int main() 
{
    int num ;
    scanf(" %d",&num);
    int result = num&num-1;
    printf("Original: %d, After clearing last set bit: %d\n", num, result);
    return 0;
}

