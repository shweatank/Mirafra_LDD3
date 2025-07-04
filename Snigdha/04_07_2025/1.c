#include <stdio.h>
int main() {
    int arr[5] = {0};
    for (int i = 0; i < 5; i++)
        arr[i] = i + arr[i - 1];
    printf("%d\n", arr[4]);
    return 0;
}
