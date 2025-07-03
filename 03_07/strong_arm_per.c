#include <stdio.h>

 //Perfect number check 
int is_perfect(int n) {
    if (n < 2) return 0;
    int sum = 1;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) {
            sum += i;
            if (i != n / i) sum += n / i;
        }
    }
    return sum == n;
}

//Factorial for strong numbers 
int factorial(int n) {
    int f = 1;
    for (int i = 2; i <= n; i++) f *= i;
    return f;
}

// Strong number check 
int is_strong(int n) {
    if (n < 0) return 0;
    int sum = 0, temp = n;
    while (temp > 0) {
        sum += factorial(temp % 10);
        temp /= 10;
    }
    return sum == n;
}

//Count digits 
int digits_count(int n) {
    if (n == 0) return 1;
    int c = 0;
    while (n) { c++; n /= 10; }
    return c;
}


int ipow(int base, int exp) {
    int result = 1;
    while (exp > 0) {
        if (exp & 1)        
            result *= base;
        base *= base;
        exp >>= 1;       
    }
    return result;
}

Armstrong number check
int is_armstrong(int n) {
    if (n < 0) return 0;
    int d = digits_count(n);
    int sum = 0, temp = n;
    while (temp > 0) {
        int digit = temp % 10;
        sum += ipow(digit, d);
        temp /= 10;
    }
    return sum == n;
}

int main(void) {
    int perfectArr[] = {3,4,5,6,7,28,9};
    int strongArr[] = {2,4,2,6,145,28,1};
    int armstrongArr[] = {22,4,21,6,153,28,11};
    int count;

    /p/erfect numbers 
    count = 0;
    printf("Perfect numbers: ");
    for (int i = 0; i < 7; i++) {
        if (is_perfect(perfectArr[i])) {
            printf("%d ", perfectArr[i]);
            count++;
        }
    }
    printf(", count= %d\n", count);

    // Strong numbers
    count = 0;
    printf("Strong numbers: ");
    for (int i = 0; i < 7; i++) {
        if (is_strong(strongArr[i])) {
            printf("%d ", strongArr[i]);
            count++;
        }
    }
    printf(", count= %d\n", count);

    // Armstrong numbers
    count = 0;
    printf("Armstrong numbers: ");
    for (int i = 0; i < 7; i++) {
        if (is_armstrong(armstrongArr[i])) {
            printf("%d ", armstrongArr[i]);
            count++;
        }
    }
    printf(", count= %d\n", count);

    return 0;
}

