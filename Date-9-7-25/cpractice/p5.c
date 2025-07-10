#include<stdio.h>
int main()
{
	int i,j,n;
	printf("Enter The Number:\n");
	scanf("%d",&n);
	int a[n];
	printf("Enter The Elements:\n");
	for(i=0;i<n;i++)
		scanf("%d",&a[i]);

	int num=0;
	for(i=0;i<n;i++)
	{
		num=num^a[i];
	}
	printf("%d\n",num);
}
