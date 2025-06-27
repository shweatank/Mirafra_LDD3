#include <stdio.h>
#include <string.h>

char findDifference(char *s, char *t)
{
    char result = 0;
    for (int i = 0; s[i]; i++) {
        result ^= s[i];
    }
    for (int i = 0; t[i]; i++) {
        result ^= t[i];
    }
    return result;
}

char findTheDifference(char *s, char *t, char (*fp)(char *, char *))
{
    return fp(s, t);
}

int main() 
{
    char s[] = "abcd";
    char t[] = "abcde";

    char result = findTheDifference(s, t, findDifference);
    printf("The extra character is: '%c'\n", result);

    return 0;
}

