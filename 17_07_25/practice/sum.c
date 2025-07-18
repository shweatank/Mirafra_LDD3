#include<stdio.h>
int main()
{
	int size,arr[32],sum,i;
	printf("Enter size of an array ");
	scanf("%d",&size);
	if(size<=0||size>32)
	{
		printf("Invalid input\n");
		return 1;
	}
	printf("Enter elements in an array ");
	for(i=0;i<size;i++)
	{
		scanf("%d",&arr[i]);
	}
	for(i=0;i<size;i++)
	{
		sum+=arr[i];
	}
	printf("%d\n",sum);
	return 0;
}
