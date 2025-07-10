#include <stdio.h>

#define MAX 10000

int main() {
    int i, n;

    scanf("%d", &n);

    int a[n], flag[MAX] = {0};

    for (i = 0; i < n; i++) {
        scanf("%d", &a[i]);
    }

    for (i = 0; i < n; i++) {
        if (flag[a[i]] == 0) {
            printf("%d ", a[i]);
            flag[a[i]] = 1;
        }
    }

    return 0;
}

