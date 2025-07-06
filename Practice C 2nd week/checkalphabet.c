#include<stdio.h>
void main()
{
	char ch,r;
	printf("Enter the character :");
	scanf("%c",&ch);
	r=((ch>='a' && ch<='z')||(ch>='A' && ch<='Z'))?1:0;
	if(r)
		printf("Yes...\n");
	else
		printf("No...\n");
}
