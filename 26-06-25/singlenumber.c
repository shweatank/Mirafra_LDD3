//26 June 2025
//To find the element in the array which doesn't repeat
#include<stdio.h>
#include<stdlib.h>
int single_element(int *arr,int size)
{
	int a=0,i;
	for(i=0;i<size;i++)
	{
		a=a^arr[i];
	}
	return a;
}
void main()
{
	int *arr,size,i,single;
	printf("Enter the size of the array : ");
	scanf("%d",&size);
	arr=(int *)malloc(sizeof(int)*size);
	printf("Enter the elements of array : \n");
	for(i=0;i<size;i++)
		scanf("%d",&arr[i]);
	single=single_element(arr,size);
	printf("Single element = %d\n",single);
	free(arr);
}
