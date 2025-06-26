//26 June 2025
//To check whether the bits of a number are alternating or not
#include<stdio.h>
int alternate_bit_check(int num)
{
	while(num)
	{
		if((num%2)^((num/2)%2))
		{
			num/=2;
		}
		else
			return 0;
	}
	return 1;
}
void main()
{
	int num;
	printf("Enter the number: ");
	scanf("%d",&num);
	if(alternate_bit_check(num))
		printf("true...\n");
	else
		printf("false...\n");
}
