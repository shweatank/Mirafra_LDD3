#include<stdio.h>
void print(int *arr,int n)
{
	for(int i=0;i<n;i++)
		printf("%d ",arr[i]);
	printf("\n");
}
void insert_element(int *arr,int n,int index,int val)
{
	if(index==n)
	{
		arr[n]=val;
		return;
	}
	for(int i=n-1;i>=index;i--)
		arr[i+1]=arr[i];
	arr[index]=val;
}
int main()
{
	int n,index,val;
	printf("enter n, index and value:\n");
	scanf("%d%d%d",&n,&index,&val);
	int arr[n+1];
	printf("enter elements:\n");
	for(int i=0;i<n;i++)
		scanf("%d",&arr[i]);
	printf("Array elements:\n");
	print(arr,n);
	insert_element(arr,n,index,val);
	print(arr,n+1);
}
