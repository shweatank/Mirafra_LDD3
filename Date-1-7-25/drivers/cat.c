#include <stdio.h>
#include <stdlib.h>

int main(int argc,char **argv)
{
    FILE *fp;
    char ch;
    fp = fopen(argv[1], "r");
    if (fp == NULL) 
    {
        perror("Error opening file");
        return 1;
    }
    printf("\n--- File Content ---\n");
    while ((ch = fgetc(fp)) != EOF) 
    {
        putchar(ch);
    }

    fclose(fp);

    return 0;
}

