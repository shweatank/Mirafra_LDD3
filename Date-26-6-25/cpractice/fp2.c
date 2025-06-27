#include <stdio.h>
#include <stdlib.h>

int is_even(int n) {
    return n % 2 == 0;
}

int is_odd(int n) {
    return n % 2 != 0;
}

int cmp_desc(const void *a, const void *b) {
    return (*(int *)b - *(int *)a);
}

int largestInteger(int num) {
    int digits[10], parity_flags[10];
    int odds[10], evens[10];
    int len = 0, o = 0, e = 0;
    int (*check_parity)(int);

    int temp = num;
    while (temp > 0) {
        int d = temp % 10;
        check_parity = is_odd;
        parity_flags[len] = check_parity(d);  // 1 if odd, 0 if even
        digits[len++] = d;
        if (check_parity(d))
            odds[o++] = d;
        else
            evens[e++] = d;
        temp /= 10;
    }

    qsort(odds, o, sizeof(int), cmp_desc);
    qsort(evens, e, sizeof(int), cmp_desc);

    int i = 0, j = 0;
    int result = 0;

    for (int idx = len - 1; idx >= 0; idx--) {
        result *= 10;
        if (parity_flags[idx])
            result += odds[i++];
        else
            result += evens[j++];
    }

    return result;
}

int main() {
    int num;
    printf("Input:\n");
    scanf("%d",&num);
    printf("Output: %d\n", largestInteger(num));
    return 0;
}

