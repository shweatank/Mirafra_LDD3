#include<stdio.h>
void print(int *arr,int n)
{
	for(int i=0;i<n;i++)
		printf("%d ",arr[i]);
	printf("\n");
}
void print_unique(int *arr,int n)
{
	int cnt=0;
	for(int i=0;i<n;i++)
	{
		cnt=0;
		for(int j=0;j<n;j++)
		{
			if(i>j && arr[i]==arr[j])
				break;
			else if(arr[i]==arr[j])
				cnt++;
		}
		if(cnt>=1)
			printf("%d ",arr[i]);
	}
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
	print_unique(arr,n);
}
