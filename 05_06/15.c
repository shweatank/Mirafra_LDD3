#include <stdio.h>
#include <string.h>

void shiftLeft(char *p) {
    if (*p != '\0') {
        *p = *(p + 1);
        shiftLeft(p + 1);
    }
}

void deleteFirstNChars(char *p, int n) {
    if (n > 0) {
        shiftLeft(p);
        deleteFirstNChars(p, n - 1);
    }
}

int main() {
    char s[20] = "yesarmstrong num";
    int n = 3;

    deleteFirstNChars(s, n);
    printf("Modified string: %s\n", s);

    return 0;
}

