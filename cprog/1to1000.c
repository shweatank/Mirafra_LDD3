#include <stdio.h>

int is_binary_number(int n) {
    while (n > 0) {
        int digit = n % 10;
        if (digit != 0 && digit != 1)
            return 0;
        n /= 10;
    }
    return 1;
}

int main() {
    int i, j;
    scanf("%d %d", &i, &j);

    for (int num = i; num <= j; num++) {
        if (is_binary_number(num)) {
            printf("%d ", num);
        }
    }

    return 0;
}

