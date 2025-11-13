#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void create_leak()
{
	char *ptr = malloc(20);
	strcpy(ptr,"Hello");
	
	char *ptr2 = malloc(10);
	strcpy(ptr2,"Byr");

	free(ptr);
	free(ptr2);
}

int main()
{
	printf("Inside Main Function\n");
	create_leak();
	printf("Done\n");
	return 0;
}
