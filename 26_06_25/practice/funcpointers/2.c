// Struct that holds a function pointer
#include <stdio.h>
typedef int (*operation_fn)(int, int);
int add(int a, int b) {
    return a + b;
}

int multiply(int a, int b) {
    return a * b;
}

typedef struct {
    char name[20];
    operation_fn op;
} Operation;

int main() {
    Operation op1 = {"Addition", add};
    Operation op2 = {"Multiplication", multiply};

    int x = 10, y = 5;

    printf("%s of %d and %d = %d\n", op1.name, x, y, op1.op(x, y));
    printf("%s of %d and %d = %d\n", op2.name, x, y, op2.op(x, y));

    return 0;
}

