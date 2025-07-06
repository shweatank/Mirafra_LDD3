#include <stdio.h>
#include <string.h>

int fact(int);
int strong(int);

void main() {
    char s[20] = "h";
    int len = strlen(s);
    int res = strong(len);
    if (res == len)
        printf("%d is a strong number.\n", len);
    else
        printf("%d is not a strong number.\n", len);
        }
int strong(int len) {
    if (len == 0) 
        return 0;
     else {
        int n = len % 10;
        return fact(n) + strong(len / 10);
    }
}
int fact(int n) {
    if (n <= 1)
        return 1;
    else
        return n * fact(n - 1);
}

