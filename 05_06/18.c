#include <stdio.h>
#include <string.h>

void insertChar(char *s, int index, char ch) {
    int len = strlen(s);
    if (len >= index) {
        s[len + 1] = '\0';         
        if (len >= index) {      
            s[len] = s[len - 1];
            insertChar(s, index, ch);
        } else {
            s[index] = ch;
        }
    }
}

int main() {
    char s[20] = "abcdef";
    int index = 2;
    char ch = '9';

    insertChar(s, index, ch);
    printf("Modified string: %s\n", s);

    return 0;
}

