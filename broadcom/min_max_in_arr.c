#include<stdio.h>
void min_max(int *arr,int n)
{
	int min=arr[0],max=arr[0];
	for(int i=1;i<n;i++)
	{
		if(arr[i]<min)
			min=arr[i];
		if(arr[i]>max)
			max=arr[i];
	}
	printf("min:%d max:%d\n",min,max);
}
int main()
{
	int n;
	printf("enter n:\n");
	scanf("%d",&n);
	int arr[n];
	for(int i=0;i<n;i++)
		scanf("%d",&arr[i]);
	min_max(arr,n);
}
