#include<stdio.h>
void main()
{
	int size=0,arr[30]={0},i=0;
	printf("Enter size of an array ");
	scanf("%d",&size);
	printf("Enter elements in an array ");
	for(;i<size;i++)
	{
		scanf("%d",&arr[i]);
	}
	for(i=0;i<size-1;i++)
	{
		int flag=0;
		for(int j=0;j<size-i-1;j++)
		{
			if(arr[j]>arr[j+1])
			{
				int temp=arr[j];
				arr[j]=arr[j+1];
				arr[j+1]=temp;
				flag=1;
			}
		}
		if(flag==0)
			break;
	}
	printf("After sorting elements in an array are ");
	for(i=0;i<size;i++)
	{
		printf("%d ",arr[i]);
	}
	printf("\n");
}
