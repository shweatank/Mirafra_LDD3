#include <stdio.h>

typedef int (*op_fn)(int, int);  

struct Calc {
    int a, b;
    op_fn fn;                    
};


int add(int x, int y) { return x + y; }
int sub(int x, int y) { return x - y; }
int mul(int x, int y) { return x * y; }
int divide(int x, int y) { return y ? x / y : 0; }

int main(void)
{
    struct Calc c;
    int choice;

    printf("Enter two integers: ");
    scanf("%d %d", &c.a, &c.b);

    printf("1) +  2) -  3) *  4) /  : ");
    scanf("%d", &choice);

    
    c.fn = (choice == 1) ? add :
           (choice == 2) ? sub :
           (choice == 3) ? mul : divide;

    printf("Result = %d\n", c.fn(c.a, c.b));
    return 0;
}

