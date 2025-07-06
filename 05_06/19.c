#include <stdio.h>
#include <string.h>

void insertCharAtZero(char *s, int len, char ch) {
    if (len < 0) {
        s[0] = ch; 
        return;
    }
    
    s[len + 1] = s[len]; 
    insertCharAtZero(s, len - 1, ch); 
}

int main() {
    char s[20] = "abcdef"; 
    char ch = '9'; 

    insertCharAtZero(s, strlen(s) - 1, ch);
    printf("Modified string: %s\n", s);

    return 0;
}

