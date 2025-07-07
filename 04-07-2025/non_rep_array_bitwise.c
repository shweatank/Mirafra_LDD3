#include<stdio.h>
int non_rep(int *arr,int n)
{
	int res=0;
	for(int i=0;i<n;i++)
	{
		res^=arr[i];
	}
	return res;
}
int main()
{
	int n;
	printf("enter n value\n");
	scanf("%d",&n);
	int arr[n];
	printf("enter elements:\n");
	for(int i=0;i<n;i++)
		scanf("%d",&arr[i]);
	int res=non_rep(arr,n);
	printf("non repeated element:%d\n",res);
}
