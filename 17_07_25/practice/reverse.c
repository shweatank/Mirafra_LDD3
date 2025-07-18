#include<stdio.h>
void main()
{
	int size,arr[32],i,temp;
	printf("Enter size of an array ");
	scanf("%d",&size);
	printf("Enter elements in an array ");
	for(i=0;i<size;i++)
	{
		scanf("%d",&arr[i]);
	}
	for(i=0;i<size/2;i++)
	{
		temp=arr[i];
		arr[i]=arr[size-1-i];
		arr[size-1-i]=temp;
	}
	printf("The reversed array is ");
	for(i=0;i<size;i++)
	{
		printf("%d ",arr[i]);
	}
	printf("\n");
}
