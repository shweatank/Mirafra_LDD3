#include<stdio.h>
void reverse_string(char *p)
{
	if(*p)
	{
		reverse_string(p+1);
		printf("%c",*p);
	}
}
int main()
{
	char s[20];
	printf("Enter The String\n");
	scanf(" %s",s);

	reverse_string(s);
	printf("\n");
}
