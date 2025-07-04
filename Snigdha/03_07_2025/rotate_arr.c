#include<stdio.h>



void reverse(int arr[],int s,int k)
{
	int temp,i,j;
	while(s<k)
	{
		temp=arr[s];
		arr[s]=arr[k];
		arr[k]=temp;
		s++;k--;
	}
}


int main()
{
	int s;
	printf("Enter size of array:");
	scanf("%d",&s);
	int arr[s];
	printf("Enter array elements:");
	for(int i=0;i<s;i++)
		scanf("%d",&arr[i]);
	int k;
	printf("Enter how many elements for rotation:");
	scanf("%d",&k);

	reverse(arr,0,s-1);

	reverse(arr,0,k-1);


	reverse(arr,k,s-1);

	printf("After rotation, array elements are:");
	for(int i=0;i<s;i++)
		printf("%d ",arr[i]);
	printf("\n");
}

