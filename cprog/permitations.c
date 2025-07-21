#include <stdio.h>
#include <string.h>

// Function to swap characters at position i and j
void swap(char *x, char *y)
{
    char temp = *x;
    *x = *y;
    *y = temp;
}

// Recursive function to generate permutations
void permute(char *str, int l, int r)
{
    if (l == r)
    {
        printf("%s\t", str);
    }
    else
    {
        for (int i = l; i <= r; i++)
        {
            swap(&str[l], &str[i]);        // Swap current index with l
            permute(str, l + 1, r);        // Recurse for the rest
            swap(&str[l], &str[i]);        // Backtrack (restore original string)
        }
    }
}

int main()
{
    char str[] = "ABCDE";
    int n = strlen(str);
    permute(str, 0, n - 1);
    return 0;
}

