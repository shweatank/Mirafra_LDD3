#include<stdio.h>
void dectobin(int num)
{
	int pos;
	for(int pos=31;pos>=0;pos--)
	{
		printf("%d",(num>>pos)&1);
		if(pos%8==0)
			printf(" ");
	}
	printf("\n");
}
void main()
{
	int num;
	printf("Enter the number : ");
	scanf("%d",&num);
	dectobin(num);
}
