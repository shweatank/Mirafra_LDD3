#include<stdio.h>
void print(int *arr,int n)
{
	for(int i=0;i<n;i++)
		printf("%d ",arr[i]);
	printf("\n");
}
int is_leader(int *arr,int n,int i)
{
	for(int j=i+1;j<n;j++)
		if(arr[j]>arr[i])
			return 0;
	return 1;
}
void print_leaders(int *arr,int n)
{
	printf("Leaders:\n");
	for(int i=0;i<n;i++)
	{
		if(is_leader(arr,n,i))
			printf("%d ",arr[i]);
	}
	printf("\n");
}
int main()
{
	int n;
	printf("enter n:\n");
	scanf("%d",&n);
	int arr[n];
	printf("enter elements:\n");
	for(int i=0;i<n;i++)
		scanf("%d",&arr[i]);
	printf("Array elements:\n");
	print(arr,n);
	print_leaders(arr,n);
}
