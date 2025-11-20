#include<stdio.h>
void bin_eq(char ch)
{
	for(int i=sizeof(ch)*8-1;i>=0;i--)
		printf("%d",(ch>>i)&1);
	printf("\n");
}
int main()
{
	char ch;
	printf("enter char:\n");
	scanf(" %c",&ch);
	bin_eq(ch);
}
