#include <stdio.h>
#include <string.h>

int count = 0;

// Function to reverse the full string recursively
void revv(char *src, char *dest)
{
    char ch;
    if (*src != '\0')
    {
        ch = *src;
        revv(src + 1, dest);
        dest[count++] = ch;
    }
    dest[count] = '\0';
}

// Function to reverse characters between two pointers (word reverse)
void wordrev(char *start, char *end)
{
    while (start < end)
    {
        char temp = *start;
        *start = *end;
        *end = temp;
        start++;
        end--;
    }
}

int main()
{
    char p[100], reve[100];
    fgets(p, 100, stdin);

    // Remove newline at end, if any
    if (p[strlen(p) - 1] == '\n')
        p[strlen(p) - 1] = '\0';

    // Step 1: Reverse whole string from p -> reve
    count = 0;  // reset global counter
    revv(p, reve);

    // Step 2: Word-wise reverse on reve
    char *ch, *p1 = reve;

    while (*p1 != '\0')
    {
        // Skip spaces
        while (*p1 == ' ')
            p1++;

        ch = p1;

        // Move p1 to end of current word
        while (*p1 != ' ' && *p1 != '\0')
            p1++;

        wordrev(ch, p1 - 1);  // Reverse the word
    }

    printf("Word-wise reversed string: %s\n", reve);
    return 0;
}

