#include <stdio.h>

int sumDigits(char *p);

int main() {
    char s[20] = "123 hello by6";
    int sum = sumDigits(s);
    printf("sum = %d\n", sum);
    return 0;
}

int sumDigits(char *p) {
    if (*p == '\0')
        return 0;
    
    if (*p >= '0' && *p <= '9')
        return (*p - '0') + sumDigits(p + 1);
    else
        return sumDigits(p + 1);
}


#include <stdio.h>

int sumDigits(char *p, int index);

int main() {
    char s[20] = "123 hello by6";
    int sum = sumDigits(s, 0);
    printf("sum = %d\n", sum);
    return 0;
}

int sumDigits(char *p, int index) {
    if (p[index] == '\0')  
        return 0;
    
    if (p[index] >= '0' && p[index] <= '9')
        return (p[index] - '0') + sumDigits(p, index + 1);
    else
        return sumDigits(p, index + 1);
}

