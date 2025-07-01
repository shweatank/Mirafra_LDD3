#include <stdio.h>

int count_set_bits(int num) {
    int count = 0;
    while (num) {
        num &= (num - 1);
        count++;
    }
    return count;
}

int main() {
    int num = 29; 
    printf("Set bits: %d\n", count_set_bits(num));
    return 0;
}

