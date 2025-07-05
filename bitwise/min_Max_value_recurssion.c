#include <stdio.h>
void findMinMax(int n, int *min, int *max)
{
    if (n == 0)
        return;

    int r = n % 10;

    if (r > *max)
        *max = r;
    if (r < *min)
        *min = r;

    findMinMax(n / 10, min, max);
}

int main()
{
    int n;
    printf("enter a number:\n");
    scanf("%d", &n);

    int max = 0, min = 9;

    if (n == 0) {
        min = max = 0;
    } else {
        findMinMax(n, &min, &max);
    }

    printf("min = %d\n", min);
    printf("max = %d\n", max);
}

