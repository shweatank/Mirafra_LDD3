#include<stdio.h>
int binary_search(int arr[],int info,int n)
{
	int l=0,r=n-1;
	while(l<=r)
	{
		int mid=(l+r)/2;
		if(arr[mid]==info)
			return mid;
		else if(info>arr[mid])
			l=mid+1;
		else
			r=mid-1;
	}
	return -1;
}
void main()
{
	int size=0,data=0,arr[30]={0};
	printf("Enter size of an array ");
	scanf("%d",&size);
	printf("Enter elements in sorted order to array ");
	for(int i=0;i<size;i++)
	{
		scanf("%d",&arr[i]);
	}
	printf("Enter an element you want to search ");
	scanf("%d",&data);
	int index=binary_search(arr,data,size);
	if(index==-1)
	{
		printf("Element not found\n");
	}
	else
	{
		printf("Element %d found at index %d\n",data,index);
	}
}
