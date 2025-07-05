#include <stdio.h>
unsigned int convert_endian(unsigned int num) {
    return ((num >> 24) & 0x000000FF) | // move byte 4 to byte 1
           ((num >> 8)  & 0x0000FF00) | // move byte 3 to byte 2
           ((num << 8)  & 0x00FF0000) | // move byte 2 to byte 3
           ((num << 24) & 0xFF000000);  // move byte 1 to byte 4
}
int main() {
    unsigned int input;

    printf("Enter a 32-bit hexadecimal number ( 0x12345678): ");
    scanf("%x", &input);
    unsigned int output = convert_endian(input);
    printf("Original (Little Endian): 0x%08X\n", input);
    printf("Converted (Big Endian) : 0x%08X\n", output);
}

