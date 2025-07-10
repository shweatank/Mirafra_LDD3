#include<stdio.h>
#define MAX 10000
int main()
{
	int i,n;
	printf("Enter THE Numbers:\n");
	scanf("%d",&n);
	int a[n];
	int count[MAX]={0};
	printf("Enter the elements:\n");
	for(i=0;i<n;i++)
	{
		scanf("%d",&a[i]);
		count[a[i]]++;
	}

	for(i=0;i<n;i++)
	{
		if(count[a[i]]>1)
		{
			printf("%d---->%d\n",a[i],count[a[i]]);
			count[a[i]]=0;
		}
	}
}
