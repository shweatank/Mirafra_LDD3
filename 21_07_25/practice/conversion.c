#include<stdio.h>
void main()
{
	char ch;
	printf("Enter a character ");
	scanf("%c",&ch);
	if((ch>='A'&&ch<='Z')||(ch>='a'&&ch<='z'))
	{
		ch=ch^(1<<5);
	}
	printf("%c\n",ch);
}
