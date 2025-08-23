#include<stdio.h>
int main()
{
	int n,freq[10]={0};
	printf("enter n value:");
	scanf("%d",&n);
	int arr[n];
	for(int i=0;i<n;i++)
		scanf("%d",&arr[i]);
	for(int i=0;i<n;i++)
	{
	 freq[arr[i]]++;
	}
	for(int i=0;i<n;i++)
	{
		if(freq[i]!=0)
			printf("%d repeated %d times\n",i,freq[i]);
	}
}
