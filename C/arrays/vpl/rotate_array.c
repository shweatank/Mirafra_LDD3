#include<stdio.h>
void print(int *arr,int n)
{
	for(int i=0;i<n;i++)
		printf("%d ",arr[i]);
	printf("\n");
}
void rotate_left(int *arr,int n,int r)
{
	int temp;
	for(int i=0;i<r;i++)
	{
		temp=arr[0];
		for(int j=0;j<n-1;j++)
			arr[j]=arr[j+1];
		arr[n-1]=temp;
	}
}
void rotate_right(int *arr,int n,int r)
{
	int temp;
	for(int i=0;i<r;i++)
	{
		temp=arr[n-1];
		for(int j=n-1;j>0;j--)
			arr[j]=arr[j-1];
		arr[0]=temp;
	}
}

int main()
{
	int n,r;
	char ch;
	printf("enter n and how many times to rotate:\n");
	scanf("%d%d",&n,&r);
	printf("enter r for right l for left:\n");
	scanf(" %c",&ch);
	int arr[n];
	printf("enter elements:\n");
	for(int i=0;i<n;i++)
		scanf("%d",&arr[i]);
	printf("Array elements:\n");
	print(arr,n);
	if(ch=='r')
		rotate_right(arr,n,r);
	else if(ch=='l')
		rotate_left(arr,n,r);
	else
		printf("invalid choice\n");
	printf("After rotating elements:\n");
	print(arr,n);
}
