#include <stdio.h>
#include <stdlib.h>

int main(int argc,char **argv) 
{
    FILE *fp;
    char data[1000];

    fp = fopen(argv[1], "w");
    if (fp == NULL) 
    {
        perror("Error opening file");
        return 1;
    }

    printf("Enter text to write into the file\n");

    getchar();  
    while (1) 
    {
        fgets(data, sizeof(data), stdin);
        if (data[0] == '~') break;
        fputs(data, fp);
    }

    fclose(fp);

    return 0;
}

