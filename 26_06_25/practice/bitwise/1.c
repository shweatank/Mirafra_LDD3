//add

#include <stdio.h>

int add(int a, int b) {
    while (b != 0) {
        int carry = (a & b) << 1; 
        a = a ^ b;                
        b = carry;               
    }
    return a;
}

int main() {
    int num1 = 15, num2 = 27;
    printf("Sum = %d\n", add(num1, num2));
    return 0;
}

