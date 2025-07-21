#include<stdio.h>
void main()
{
	int size,arr[32],i,j;
	printf("Enter size of an array ");
	scanf("%d",&size);
	printf("Enter elements in an array ");
	for(i=0;i<size;i++)
	{
		scanf("%d",&arr[i]);
	}
	for(i=0;i<size-1;i++)
	{
		for(j=0;j<size-1-i;j++)
		{
			if(arr[j]>arr[j+1])
			{
				int temp=arr[j];
				arr[j]=arr[j+1];
				arr[j+1]=temp;
			}
		}
	}
	for(i=0;i<size/2;i++)
	{
		printf("%d ",arr[i]);
	}
	for(j=size-1;j>=size/2;j--)
	{
		printf("%d ",arr[j]);
	}
	printf("\n");
}
