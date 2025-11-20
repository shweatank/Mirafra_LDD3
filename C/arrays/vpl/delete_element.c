#include<stdio.h>
void print(int *arr,int n)
{
	for(int i=0;i<n;i++)
		printf("%d ",arr[i]);
	printf("\n");
}
void delete_element(int *arr,int n,int index)
{
	if(index<0 || index>=n)
	{
		printf("invalid index\n");
		return;
	}
	for(int i=index;i<n-1;i++)
		arr[i]=arr[i+1];
}
int main()
{
	int n,index;
	printf("enter n, index :\n");
	scanf("%d%d",&n,&index);
	int arr[n];
	printf("enter elements:\n");
	for(int i=0;i<n;i++)
		scanf("%d",&arr[i]);
	printf("Array elements:\n");
	print(arr,n);
	delete_element(arr,n,index);
	print(arr,n-1);
}
