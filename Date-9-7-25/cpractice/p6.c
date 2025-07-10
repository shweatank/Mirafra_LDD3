#include<stdio.h>
int main()
{
	int i,j,n,c,k=0;
	printf("Enter The Nunber:\n");
	scanf("%d",&n);

	int a[n];
	int b[n];
	printf("Enter THe Elements:\n");
	for(i=0;i<n;i++)
		scanf("%d",&a[i]);

	for(i=0;i<n;i++)
	{
       		c=0;
		for(j=0;j<n;j++)
		{
			if(a[i]==b[j])
				c=1;
		}
		if(c==0)
			b[k++]=a[i];
	}
	for(i=0;i<k;i++)
		printf("%d",b[i]);
}

