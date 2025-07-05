#include<stdio.h>
int main()
{
	int num,i=0,digit;
	char arr[10];
	printf("enter the num\n");
	scanf("%d",&num);
	while(num)
	{
		digit=num%16;
		digit=(digit>9) ? (digit-10)+'A': digit+48;
		arr[i]=digit;
		num=num/16;
		i++;
	}
	for(--i;i>=0;i--)
	{
		printf("%c",arr[i]);
	}
}
		

