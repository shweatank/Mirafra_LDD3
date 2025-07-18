#include<stdio.h>
int main()
{
	int size=0,arr[32]={0},smallest=0;
	printf("Enter size of an array ");
	scanf("%d",&size);
	if(size<0||size>32)
	{
		printf("Invalid input\n");
		return 1;
	}
	printf("Enter elements in an array ");
	for(int i=0;i<size;i++)
	{
		scanf("%d",&arr[i]);
	}
	smallest=arr[0];
	for(int i=1;i<size;i++)
	{
		if(smallest>arr[i])
			smallest=arr[i];
	}
	printf("%d\n",smallest);
	return 0;
}
