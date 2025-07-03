#include <stdio.h>

// Function to print 32-bit binary representation
void print_binary(unsigned int num)
{
    for (int bit = 31; bit >= 0; bit--)
    {
        printf("%d", (num >> bit) & 1);
        if (bit % 8 == 0)  // Add space every 8 bits
            printf(" ");
    }
    printf("\n");
}

// Function to toggle bits from i to j (inclusive)
unsigned int to_toggle(unsigned int num, int i, int j)
{
    unsigned int mask;

    // Handle full 0–31 range separately to avoid undefined behavior from 1 << 32
    if (i == 0 && j == 31)
    {
        mask = 0xFFFFFFFF;  // All 32 bits set
    }
    else
    {
        mask = ((1U << (j - i + 1)) - 1) << i;
    }

    return num ^ mask;  // Toggle bits using XOR
}

int main()
{
    unsigned int num;
    int i, j;

    // Input
    printf("Enter num, i, and j: ");
    scanf("%u %d %d", &num, &i, &j);

    // Check for valid range
    if (i < 0 || j > 31 || i > j)
    {
        printf("Invalid range! i should be <= j, and both in [0, 31].\n");
        return 1;
    }

    // Before toggling
    printf("\nBefore toggling:\n");
    print_binary(num);

    // Perform toggle
    num = to_toggle(num, i, j);

    // After toggling
    printf("\nAfter toggling bits %d to %d:\n", i, j);
    print_binary(num);

    return 0;
}

