#include <stdio.h>

// Recursive function to check for prime
int is_prime(int n, int i) {
    if (n <= 2)
        return (n == 2); // 2 is prime, 1 and below are not
    if (n % i == 0)
        return 0;        // divisible => not prime
    if (i * i > n)
        return 1;        // no divisors up to sqrt(n) => prime
    return is_prime(n, i + 1); // check next divisor
}

int main() {
    int num;
    printf("Enter a number: ");
    scanf("%d", &num);

    if (is_prime(num, 2))
        printf("%d is a Prime Number\n", num);
    else
        printf("%d is NOT a Prime Number\n", num);

    return 0;
}

