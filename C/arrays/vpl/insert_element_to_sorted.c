#include<stdio.h>
void print(int *arr,int n)
{
	for(int i=0;i<n;i++)
		printf("%d ",arr[i]);
	printf("\n");
}
void insert_element(int *arr,int n,int val)
{
	if(val>arr[n-1])
	{
		arr[n]=val;
		return;
	}
	int i;
	for(i=0;i<n-1;i++)
	{
		if(val>=arr[i] && val<=arr[i+1])
			break;
	}
	for(int j=n-1;j>=i;j--)
		arr[j+1]=arr[j];
	arr[i+1]=val;
}

int main()
{
	int n,val;
	printf("enter n and val to insert:\n");
	scanf("%d%d",&n,&val);
	int arr[n+1];
	printf("enter elements:\n");
	for(int i=0;i<n;i++)
		scanf("%d",&arr[i]);
	printf("Array elements:\n");
	print(arr,n);
	insert_element(arr,n,val);
	print(arr,n+1);
}
