#include<stdio.h>
void main()
{
	int num,pos=-1,i;
	printf("Enter the number : ");
	scanf("%d",&num);
	for(i=31;i>=0;i--)
		if((num>>i)&1)
		{
			pos=i;
			break;
		}
	if(pos==-1)
		printf("no set bit\n");
	else
		printf("%d has highest bit at %d position \n",num,pos);
}

