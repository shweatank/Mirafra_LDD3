#include<stdio.h>
void main()
{
	int num,pos,i,j,m,n;
	printf("Enter the number : ");
	scanf("%d",&num);
	printf("Before...\n");
	for(pos=31;pos>=0;pos--)
	{
		printf("%d ",(num>>pos)&1);
	}
	printf("\n");
	for(i=0;i<31;i+=2)
	{
		if(((num>>i)&1) != ((num>>(i+1)&1)))
		{
			num=num^(1<<i);
			num=num^(1<<(i+1));
		}
	}
	printf("After...\n");
	for(pos=31;pos>=0;pos--)
		printf("%d ",(num>>pos)&1);
	printf("\n");
}
