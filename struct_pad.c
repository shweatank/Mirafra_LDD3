#include <stdio.h>

struct A {
    char a;   // 1 byte
    int b;    // 4 bytes (needs to be aligned at a multiple of 4)
    char c;   // 1 byte
};

int main() {
    struct A obj;
    printf("Size of struct A: %lu\n", sizeof(obj));
    return 0;
}

