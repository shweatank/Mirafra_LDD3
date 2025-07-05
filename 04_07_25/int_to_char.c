#include<stdio.h>
#include<string.h>
int main()
{
	int num,i=0,flag=0;
	char arr[4];
	printf("enter the num:\n");
	scanf("%d",&num);
	if(num<0)
	{
		num=-num;
		flag=1;
	}
	while(num)
	{
		arr[i]=(num%10)+48;
		num=num/10;
		i++;
	}
	for(--i;i>=0;i--)
	{
		printf("%c",arr[i]);
	}
	printf("\n");
	puts(arr);
}
		
