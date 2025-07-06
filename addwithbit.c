#include <stdio.h>

// Function to add two integers using bitwise operators
int add(int a, int b) {
	int count=0;
    while (b != 0) {
        // carry contains common set bits of a and b
        int carry = (a & b) << 1;

        // sum of bits where at least one of the bits is not set
        a = a ^ b;

        // carry is added to a in the next iteration
        b = carry;
	count++;
    }
    printf("count = %d \n",count);
    return a;
}

int main() {
    int num1, num2;

    printf("Enter two integers: ");
    scanf("%d %d", &num1, &num2);

    int result = add(num1, num2);

    printf("Sum = %d\n", result);

    return 0;
}

