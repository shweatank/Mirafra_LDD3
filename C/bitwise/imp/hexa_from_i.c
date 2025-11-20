#include<stdio.h>
void hexa(int num,int i)
{
	num=(num>>i)&0x0f;
	if(num<=9)
		printf("%d\n",num);
	else
		printf("%c\n",(num-10)+'A');
}
void print_binary(int num)
{
	for(int i=31;i>=0;i--)
	{
		printf("%d",(num>>i)&1);
		if(i%8==0)
			printf(" ");
	}
	printf("\n");
}
int main()
{
	int num,i;
	printf("enter num and i:\n");
	scanf("%d%d",&num,&i);
	print_binary(num);
	hexa(num,i);
}
