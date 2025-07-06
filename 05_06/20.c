#include <stdio.h>
#include <string.h>

void insertChar(char *s, int len, char ch, int index) {
    if (len < index) {
        return;
    }
    
    s[len + 1] = s[len];

    if (len == index) {
        s[index] = ch; 
        return; 
    }

    insertChar(s, len - 1, ch, index);
}

int main() {
    char s[20] = "123456";
    insertChar(s, strlen(s), 'p', 0);
    insertChar(s, strlen(s), 'q', 1);
    
    printf("Modified string: %s\n", s);

    return 0;
}

