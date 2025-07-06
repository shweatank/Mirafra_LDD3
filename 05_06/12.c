#include <stdio.h>
void shiftLeft(char *p) {
    if (*p == '\0') {
        return;
    }
    *p = *(p + 1);
    shiftLeft(p + 1); 
}
void deleteChar(char *p, char ch) {
    if (*p == '\0') {
        return;
    }

    if (*p == ch) {
        shiftLeft(p);
        deleteChar(p, ch);
    } else {
        deleteChar(p + 1, ch);
    }
}

int main() {
    char s[20] = "hello world";
    char ch = 'l';
    deleteChar(s, ch);
    printf("%s\n", s);
    return 0;
}

