#include <stdio.h>

int main() {
    FILE *fp;
    char buffer[100];

    // Open the file in read mode
    fp = fopen("/dev/char_dev", "r");

    // Check if file opened successfully
    if (fp == NULL) {
        printf("Error opening file!\n");
        return 1;
    }

    // Read the file line by line
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("%s", buffer);  // Print each line
    }

    // Close the file
    fclose(fp);

    return 0;
}

