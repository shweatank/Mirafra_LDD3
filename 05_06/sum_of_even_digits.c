#include <stdio.h>

int sumEvenDigits(int num);

void main() {
    int num = 123456;
    int result = sumEvenDigits(num);
    printf("Sum of even digits: %d\n", result);
}

int sumEvenDigits(int num) {
    if (num == 0)
        return 0;
    
    int digit = num % 10;
    if (digit % 2 == 0)
        return digit + sumEvenDigits(num / 10);
    else
        return sumEvenDigits(num / 10);
}

//sum of all digits
#include <stdio.h>

int sumOfDigits(int num);

void main() {
    int num = 123456;
    int result = sumOfDigits(num);
    printf("Sum of all digits: %d\n", result);
}

int sumOfDigits(int num) {
    if (num == 0)
        return 0;
    
    return (num % 10) + sumOfDigits(num / 10);
}

//count of digits
#include <stdio.h>

int countDigits(int num);

void main() {
    int num = 123456;
    int result = countDigits(num);
    printf("Count of all digits: %d\n", result);
}

int countDigits(int num) {
    if (num == 0)
        return 0;
    
    return 1 + countDigits(num / 10);
}

