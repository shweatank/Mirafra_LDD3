#include <stdio.h>
#include <string.h>

void shift(char *p, int i) {
    if (p[i] != '\0') {
        p[i] = p[i + 1];
        shift(p, i + 1);
    }
}

int isprime(int n) {
    static int k = 1, c = 0;
    if (k <= n) {
        if (n % k == 0)
            c++;
        k++;
        isprime(n);
    } else {
        int res = (c == 2) ? 1 : 0; 
        k = 1; 
        c = 0;
        return res;
    }
}

void delete(char *p, int i) {
    if (p[i] != '\0') {
        if (p[i] >= '0' && p[i] <= '9') {
            int digit = p[i] - '0';
            if (isprime(digit)) {
                shift(p, i); 
               delete(p, i);
            } else {
                delete(p, i + 1);
            }
        } else {
            delete(p, i + 1);
        }
    }
}

int main() {
    char s[20] = "hello1357";
    delete(s, 0);
    printf("Modified string: %s\n", s);
    return 0;
}

