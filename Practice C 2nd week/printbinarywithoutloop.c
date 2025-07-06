#include<stdio.h>
void dectobin(int num)
{
	static int pos=31;
	if(pos>=0)
	{
		printf("%d",(num>>pos)&1);
		if(pos%8==0)
			printf(" ");
		pos--;
		dectobin(num);
	}
}
void main()
{
	int num;
	printf("Enter the number : ");
	scanf("%d",&num);
	dectobin(num);
}
