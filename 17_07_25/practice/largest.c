#include<stdio.h>
void main()
{
	int size=0,arr[32]={0},largest=0;
	printf("Enter size of an array ");
	scanf("%d",&size);
	printf("Enter elements in an array ");
	for(int i=0;i<size;i++)
	{
		scanf("%d",&arr[i]);
	}
	largest=arr[0];
	for(int i=1;i<size;i++)
	{
		if(arr[i]>largest)
			largest=arr[i];
	}
	printf("%d\n",largest);
}
