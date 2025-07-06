#include<stdio.h>
int sum(int *arr,int size)
{
	static int i=0;
	if(i<size)
		return arr[i++]+sum(arr,size);
	else
		return 0;
}
void main()
{
	int a[5];
	printf("Enter array elements: ");
	for(int i=0;i<5;i++)
		scanf("%d",&a[i]);
	printf("%d\n",sum(a,5));
}
