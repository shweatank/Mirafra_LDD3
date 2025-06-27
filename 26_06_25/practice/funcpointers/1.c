//typedef the function pointer
#include <stdio.h>

typedef int (*operation_fn)(int, int);

int add(int a, int b) {
    return a + b;
}

int subtract(int a, int b) {
    return a - b;
}

int multiply(int a, int b) {
    return a * b;
}

int divide(int a, int b) {
    if (b == 0) {
        printf("Error: Divide by zero!\n");
        return 0;
    }
    return a / b;
}

void perform_operation(operation_fn op, int x, int y) {
    printf("Result: %d\n", op(x, y));
}

int main() {
    int a = 20, b = 5;

    perform_operation(add, a, b);       
    perform_operation(subtract, a, b);  
    perform_operation(multiply, a, b);  
    perform_operation(divide, a, b);    

    return 0;
}

