#include<stdio.h>
void print(int *arr,int n)
{
	for(int i=0;i<n;i++)
		printf("%d ",arr[i]);
	printf("\n");
}
void place_zeros_end(int *arr,int n)
{
	int j=0;
	for(int i=0;i<n;i++)
	{
		if(arr[i]!=0)
			arr[j++]=arr[i];
	}
	while(j<n)
		arr[j++]=0;
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
	place_zeros_end(arr,n);
	printf("after change:\n");
	print(arr,n);
}
