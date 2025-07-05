
#include <stdio.h>
#define ByteSwap16(Value) (((Value & 0x00FF) << 8) | ((Value & 0xFF00) >> 8))
int main() {
    unsigned short original = 0x1234;
    unsigned short swapped = ByteSwap16(original);
    printf("Original: 0x%04X\n", original);
    printf("Swapped : 0x%04X\n", swapped);

    return 0;
}

