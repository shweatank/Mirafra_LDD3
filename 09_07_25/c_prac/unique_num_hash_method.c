//o(n)
#include<stdio.h>
int main()
{
	int n,res=0,cnt=0;
	printf("enter n value:\n");
	scanf("%d",&n);
	int arr[n];
	int freq[10]={0};
	for(int i=0;i<n;i++)
	{
		scanf("%d",&arr[i]);
		freq[arr[i]]++;
	}
	for(int i=0;i<n;i++)
	{
	  if(freq[arr[i]]==1)
		  printf("%d ",arr[i]);
	}
}
