//Add two numbers without + operator 
#include<stdio.h>

int main()
{
    int n1, n2;
    scanf("%d%d", &n1, &n2);

    if(n2 > 0) {
        while(n2 != 0) {              //another logic : int add= n1-~n2-1; 
            ++n1;
            --n2;
        }
    }
    else {
        while(n2 != 0) {
            --n1;
            ++n2;
        }
    }

    printf("%d", n1);
    return 0;
}

