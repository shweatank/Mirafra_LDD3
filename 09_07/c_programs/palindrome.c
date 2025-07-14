#include <stdio.h>

// Recursive function to reverse number
int reverse_number(int n, int rev) {
    if (n == 0)
        return rev;
    return reverse_number(n / 10, rev * 10 + n % 10);
}

// Main function to check palindrome
int main() {
    int num;
    printf("Enter a number: ");
    scanf("%d", &num);

    if (num == reverse_number(num, 0))
        printf("%d is a Palindrome Number\n", num);
    else
        printf("%d is NOT a Palindrome Number\n", num);

    return 0;
}

