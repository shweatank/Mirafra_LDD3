//25 JUNE 2025
//SORTING USING FUNCTION POINTER
#include<stdio.h>
#include<stdlib.h>
typedef void(*sortfp)(int *,int);
void sort(int *arr,int size)
{
	int i,j,t;
	for(i=0;i<size-1;i++)
	{
		for(j=i+1;j<size;j++)
		{
			if(arr[i]>arr[j])
			{
				t=arr[i];
				arr[i]=arr[j];
				arr[j]=t;
			}
		}
	}
}
void main()
{
	int *arr,ele;
	printf("Enter the number of elements : ");
	scanf("%d",&ele);
	arr=(int *)malloc(sizeof(int)*ele);
	printf("Enter the aray elements : ");
	for(int i=0;i<ele;i++)
		scanf("%d",&arr[i]);
	printf("Before sort : ");
	for(int i=0;i<ele;i++)
		printf("%d  ",arr[i]);
	printf("\n");
	sortfp fp=sort;
	(*fp)(arr,ele);
	printf("After sort : \n");
	for(int i=0;i<ele;i++)
	{
		printf("%d  ",arr[i]);
	}
	printf("\n");
	free(arr);
}
