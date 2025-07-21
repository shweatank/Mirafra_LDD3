#include<stdio.h>

int myatoi(char *p)
{
	int sum=0,flag=0;
	if(p[0]=='-')
	{
		p++;
		flag=1;
	}
	while(*p)
	{
		if(*p>='0' && *p<='9')
		{
			sum=(sum*10)+(*p-48);
		}
		else
		{
			printf("invalid parameter\n");
			return 0;
		}
		p++;
	}
	if(flag==1)
	{
		//sum=sum*(-1);
		//sum=-sum;
		sum=~(sum)+1;
	}
	return sum;
}

int main()
{
	int a;
	char str[10];
	gets(str);
	a=myatoi(str);
	printf("%d\n",a);
	return 0;
}
