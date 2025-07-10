#include<stdio.h>
void main()
{
	int size=0,data=0,arr[30]={0};
	printf("Enter size of an array ");
	scanf("%d",&size);
	printf("Enter elements in an array ");
	for(int i=0;i<size;i++)
	{
		scanf("%d",&arr[i]);
	}
	printf("Enter an element you want to search ");
	scanf("%d",&data);
	for(int i=0;i<size;i++)
	{
		if(data==arr[i])
		{
			printf("Element %d found at %d index\n",data,i);
			return;
		}
	}
	printf("Element not found\n");
}
