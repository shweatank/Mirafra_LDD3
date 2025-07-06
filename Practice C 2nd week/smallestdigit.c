#include<stdio.h>
#include<limits.h>
void main()
{
	int num;
	int s=INT_MAX;
	int ss=INT_MAX;
	printf("Enter the number: ");
	scanf("%d",&num);
	while(num)
	{
		if(num%10 < s)
		{
			ss=s;
			s=num%10;
		}
		else if( num%10 <ss && num%10 !=s)
			ss=num%10;
		num/=10;
	}
	if(ss==INT_MAX)
		printf("Smallest==%d\n",s);
	else
		printf("Smallest = %d\nSecond smallest  %d\n",s,ss);
}
