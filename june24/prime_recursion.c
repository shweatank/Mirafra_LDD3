#include <stdio.h>
int is_prime(int n, int i) 
{
    if (n < 2) 
	    return 0;
    if (i==1) 
	    return 1;
    if (n % i == 0) 
	    return 0;
    return is_prime(n, i-1);
}
void print_primes(int start, int end) 
{
    if (start > end) 
	    return;
    if (is_prime(start, start/2))
    {
        printf("%d ", start);
    }
    print_primes(start + 1, end);
}
int main() {
    int start, end;
    printf("Enter start of range: ");
    scanf("%d", &start);
    printf("Enter end of range: ");
    scanf("%d", &end);
    printf("Prime numbers between %d and %d:\n", start, end);
    print_primes(start, end);

    return 0;
}

