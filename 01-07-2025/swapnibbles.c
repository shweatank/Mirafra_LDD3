#include <stdio.h>
#include <stdint.h>

int main() {
    uint32_t num, result = 0;
    int i;

    printf("Enter the hex number : ");
    scanf("%x", &num);

    for (i = 0; i < 4; i++) {
        result |= ((((num >> (i * 8)) & 0xF0) >> 4) | (((num >> (i * 8)) & 0x0F) << 4)) << (i * 8);
    }

    printf("Result : %x\n", result);

    return 0;
}

