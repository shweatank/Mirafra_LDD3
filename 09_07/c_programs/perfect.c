#include <stdio.h>

// Recursive function to calculate sum of proper divisors
int sum_of_divisors(int n, int i) {
    if (i == 1)
        return 1;  // 1 is a divisor of every number
    if (n % i == 0)
        return i + sum_of_divisors(n, i - 1);
    else
        return sum_of_divisors(n, i - 1);
}

// Main function to check perfect number
int main() {
    int num;
    printf("Enter a number: ");
    scanf("%d", &num);

    if (sum_of_divisors(num, num / 2) == num)
        printf("%d is a Perfect Number\n", num);
    else
        printf("%d is NOT a Perfect Number\n", num);

    return 0;
}

