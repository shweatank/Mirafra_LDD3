#include<stdio.h>

int main()
{
	FILE *fptr;

	fptr = fopen("myfile.txt", "r");
	if(!fptr)
	{
	    printf("Error opening a file\n");	    
	    return 1;
	}

        int ch;
        while ((ch = fgetc(fptr)) != EOF) {
            printf("%c", ch);
        }

	printf("\n");

	fclose(fptr);

	return 0;
}

