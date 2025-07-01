#include<stdio.h>
int main()
{
	int num=0,flag=0;
	printf("Enter a number ");
	scanf("%d",&num);
	for(int i=2;num>1;i++)
	{
		while(num%i==0)
		{
			printf("%d\t",i);
			num/=i;
			flag=1;
		}
	}
	if(flag==0)
	{
		printf("No factors\n");
		return -1;
	}
	printf("\n");
	return 0;
}
	
