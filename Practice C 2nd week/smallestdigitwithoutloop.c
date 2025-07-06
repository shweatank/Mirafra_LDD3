#include<stdio.h>
#include<limits.h>
int s=INT_MAX;
int ss=INT_MAX;
void function(int num)
{
	//static iint s[2]={INT_MAX,INT_MAX};
	if(num)
	{
		if(num%10 < s)
		{
			ss=s;
			s=num%10;
		}
		else if(num%10<ss && num%10 !=s)
			ss=num%10;
		function(num/10);
	}
	else
		return ;
}
void main()
{
	int num;
	printf("Enter the number : ");
	scanf("%d",&num);
	function(num);
	if(ss!=INT_MAX)
		printf("s= %d ss= %d\n",s,ss);
	else
		printf("s=%d\n",s);
}
