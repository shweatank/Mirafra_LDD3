#include<stdio.h>
int missing(int *arr,int n)
{
	int res1=0,res2=0;
	for(int i=1;i<=n;i++)
		res1^=i;
	for(int i=0;i<n-1;i++)
		res2^=arr[i];
	return res1^res2;
}
int main()
{
	int n;
	printf("enter n value:\n");
	scanf("%d",&n);
	int arr[n-1];
	printf("enter %d elements:\n",n-1);
	for(int i=0;i<n-1;i++)
		scanf("%d",&arr[i]);
	int res=missing(arr,n);
	printf("missing element:%d\n",res);
}

