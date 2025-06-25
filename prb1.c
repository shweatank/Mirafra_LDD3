#include <stdio.h>

void removeDuplicates(char *s) {
    int k = 0, i, j;
    for (i = 0; s[i]; i++) {
        int dup = 0;
        for (j = 0; j < i; j++) {
            if (s[i] == s[j]) {
                dup = 1;
                break;
            }
        }
        if (!dup)
            s[k++] = s[i];
    }
    s[k] = '\0';
}

void (*fun())(char*) {
    return removeDuplicates;
}

int main() {
    char s[50];
    printf("enter string\n");
    scanf("%[^\n]", s);
    void (*p)(char*);
    p = fun();
    (*p)(s);
    printf("%s\n", s);
    
}

