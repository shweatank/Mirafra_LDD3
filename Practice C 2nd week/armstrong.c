#include<stdio.h>
int armstrong(int num)
{
	int r,sum=0,t=num,f,c,d,p;
	for(t=num,c=0;t;t/=10,c++);
	t=num;
	while(t)
	{
		r=t%10;
		for(d=c,p=1;d;d--)
			p*=r;
		sum+=p;
		t/=10;
	}
	if(sum==num)
		return 1;
	else
		return 0;
}
void main()
{
	int num;
	printf("Enter the number : ");
	scanf("%d",&num);
	if(armstrong(num))
		printf("Yes...\n");
	else
		printf("No...\n");
}
