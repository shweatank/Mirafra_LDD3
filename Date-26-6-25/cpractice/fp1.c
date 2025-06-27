#include <stdio.h>

int divideByTwo(int n) {
    return n / 2;
}

int subtractOne(int n) {
    return n - 1;
}

int numberOfSteps(int num) {
    int steps = 0;
    int (*operation)(int);

    while (num > 0) {
        if (num % 2 == 0)
            operation = divideByTwo;
        else
            operation = subtractOne;

        num = operation(num);
        steps++;
    }

    return steps;
}

int main() {
    int num = 14;
    printf("Input: %d\n", num);
    printf("Output: %d\n", numberOfSteps(num));
    return 0;
}

