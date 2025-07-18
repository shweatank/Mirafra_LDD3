#include<stdio.h>
int main()
{
	int size=0,arr[32],second_smallest,smallest;
	printf("Enter size of an array ");
	scanf("%d",&size);
	if(size<=0||size>32)
	{
		printf("Invalid input\n");
		return 1;
	}
	printf("Enter elements in an array ");
	for(int i=0;i<size;i++)
	{
		scanf("%d",&arr[i]);
	}
	if(size==1)
	{
		smallest=second_smallest=arr[0];
	}
	else
	{
		if(arr[0]>arr[1])
		{
			smallest=arr[1];
			second_smallest=arr[0];
		}
		else
		{
			smallest=arr[0];
			second_smallest=arr[1];
		}
		for(int i=2;i<size;i++)
		{
			if(arr[i]<smallest)
			{
				second_smallest=smallest;
				smallest=arr[i];
			}
			else if(second_smallest>arr[i]&&smallest<arr[i])
			{
				second_smallest=arr[i];
			}
		}
		printf("%d\n",second_smallest);
	}
	return 0;
}
