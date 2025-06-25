
//Find the only non-repeating element in an array
#include<stdio.h>
int findunique(int arr[],int n)
{
	int result=0,i;
	for(i=0;i<n;i++)
	{
		result^=arr[i];
	}
	return result;
}
int main()
{
	int arr[7];
	int n=sizeof(arr)/sizeof(arr[0]);
	printf("Enter the elements:");
	for(int i=0;i<n;i++)
		scanf("%d",&arr[i]);
	int unique=findunique(arr,n);
	printf("The non-repeating element is :%d\n",unique);
}

