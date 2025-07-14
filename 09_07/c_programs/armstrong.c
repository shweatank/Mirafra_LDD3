#include <stdio.h>

// Recursive function to count number of digits
int count_digits(int n) {
    if (n == 0)
        return 0;
    return 1 + count_digits(n / 10);
}

// Recursive function to calculate base^exp
int power(int base, int exp) {
    if (exp == 0)
        return 1;
    return base * power(base, exp - 1);
}

// Recursive function to calculate sum of digits raised to power
int armstrong_sum(int n, int power_val) {
    if (n == 0)
        return 0;
    int digit = n % 10;
    return power(digit, power_val) + armstrong_sum(n / 10, power_val);
}

// Main function to check Armstrong number
int main() {
    int num;
    printf("Enter a number: ");
    scanf("%d", &num);

    int digits = count_digits(num);
    int sum = armstrong_sum(num, digits);

    if (sum == num)
        printf("%d is an Armstrong Number\n", num);
    else
        printf("%d is NOT an Armstrong Number\n", num);

    return 0;
}

