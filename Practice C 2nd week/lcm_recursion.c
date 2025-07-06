#include <stdio.h>

int multiple = 0;

int lcm(int a, int b) {
    multiple += (a > b) ? a : b;
    if (multiple % a == 0 && multiple % b == 0)
        return multiple;
    else
        return lcm(a, b);
}

int main() {
    int num1, num2, result;

    printf("Enter two numbers: ");
    scanf("%d %d", &num1, &num2);

    multiple = 0;  // Reset before calling lcm()
    result = lcm(num1, num2);

    printf("LCM of %d and %d = %d\n", num1, num2, result);

    return 0;
}

