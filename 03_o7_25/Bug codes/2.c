#include <stdio.h>

int main() {
    char str[5];
    strcpy(str, "hello");  // Bug: writing 6 bytes into 5-byte array
    printf("%s\n", str);
    return 0;
}

//answer
//char str[10];
//strcpy(str,"hello");
