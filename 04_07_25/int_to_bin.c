#include<stdio.h>
int main()
{
	int  num,bit=31;
	printf("entre the num\n");
	scanf("%d",&num);
	while(bit>=0)
	{
		printf("%d",(num>>bit)&1);
		if(bit%8==0 && bit!=0)
			printf(" ");
		bit--;
	}
}


