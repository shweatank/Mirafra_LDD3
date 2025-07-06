#include <stdio.h>
void shiftLeft(char *p) {
    if (*p == '\0') {
        return;
    }
    *p = *(p + 1);
    shiftLeft(p + 1); 
}
void deleteChar(char *p, char ch,int n) {
    if (*p == '\0') {
        return;
    }

    if (*p == ch && n>0) {
        shiftLeft(p);
        n--;
        deleteChar(p, ch,n);
    } else {
        deleteChar(p + 1, ch,n);
    }
}
int main() {
    char s[20] = "hello world";
    char ch = 'l';
    deleteChar(s, ch,2);
    printf("%s\n", s);
    return 0;
}

