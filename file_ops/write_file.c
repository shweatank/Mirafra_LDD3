#include<stdio.h>

int main()
{
	FILE *fptr;

	fptr = fopen("myfile.txt", "w");
	if(!fptr)
	{
	    printf("Error opening a file\n");	    
	    return 1;
	}

	fprintf(fptr, "Hello, I am learning linux file operations");
	
	fclose(fptr);

	printf("File written successfully\n");

	return 0;
}

