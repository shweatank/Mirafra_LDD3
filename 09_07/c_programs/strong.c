#include <stdio.h>

// Recursive function to calculate factorial of a digit
int factorial(int n) {
    if (n <= 1)
        return 1;
    return n * factorial(n - 1);
}

// Recursive function to calculate sum of factorials of digits
int sum_of_fact(int num) {
    if (num == 0)
        return 0;
    return factorial(num % 10) + sum_of_fact(num / 10);
}

// Main function to check strong number
int main() {
    int num;
    printf("Enter a number: ");
    scanf("%d", &num);

    if (num == sum_of_fact(num))
        printf("%d is a Strong Number\n", num);
    else
        printf("%d is NOT a Strong Number\n", num);

    return 0;
}

