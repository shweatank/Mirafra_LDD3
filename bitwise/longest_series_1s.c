#include <stdio.h>

int main() {
    int n;
    scanf("%d", &n);

    int max = 0, curr = 0;

    for (int i = 0; i < 32; i++) {
        if ((n >> i) & 1)
            curr++;
        else {
            if (curr > max)
                max = curr;
            curr = 0;
        }
    }


    if (curr > max)
        max = curr;

    printf("%d\n", max);
    return 0;
}

