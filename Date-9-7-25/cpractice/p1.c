#include<stdio.h>
int main()
{
	int n,i,j;
	printf("Enter The No Of Elements :\n");
	scanf("%d",&n);
	int a[n];
	printf("Enter The Elements:\n");
	for(i=0;i<n;i++)
		scanf("%d",&a[i]);
	int count=0;
	for(i=0;i<n;i++)
	{
		for(j=0;j<i;j++)
		{
			if(a[i]==a[j])
			break;
		}
		if(i==j)
		{
			count=1;
			for(j=i+1;j<n;j++)
			{
				if(a[i]==a[j])
					count++;
			}
			printf("%d--->%d\n",a[i],count);
		}
	}
}
