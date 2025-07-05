#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int binaryToOctal(char *binary) {
    int len = strlen(binary);
    int decimal = 0, octal = 0, base = 1;

    // Convert binary to decimal
    for(int i = len - 1; i >= 0; i--) {
        if(binary[i] == '1') {
            decimal += base;
        }
        base *= 2;
    }
    printf("decimal:%d\n",decimal);

    // Convert decimal to octal
    base = 1;
    while (decimal != 0) {
        octal += (decimal % 8) * base;
        decimal /= 8;
        base *= 10;
    }

    return octal;
}

int main() {
    char binary[100];
    printf("Enter binary: ");
    scanf("%s", binary);

    printf("Octal: %d\n", binaryToOctal(binary));
    return 0;
}

