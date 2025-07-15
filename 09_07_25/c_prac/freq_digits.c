#include<stdio.h>
int main()
{
	int num,digit;
	printf("enter a num:");
	scanf("%d",&num);
	int freq[10]={0};
	while(num)
	{
		digit=num%10;
		freq[digit]++;
		num/=10;
	}
	for(int i=0;i<10;i++)
	{
		printf("%d repeats %d times\n",i,freq[i]);
	}

}
