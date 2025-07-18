#include<stdio.h>
int main()
{
	int size=0,arr[32],largest,smallest;
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
		largest=smallest=arr[0];
	}
	else
	{
		if(arr[0]>arr[1])
		{
			largest=arr[0];
			smallest=arr[1];
		}
		else
		{
			largest=arr[1];
			smallest=arr[0];
		}
		for(int i=2;i<size;i++)
		{
			if(arr[i]>largest)
			{
				largest=arr[i];
			}
			if(arr[i]<smallest)
			{
				smallest=arr[i];
			}
		}
		printf("largest %d\nsmallest %d\n",largest,smallest);
		return 0;
	}
}
		
			
