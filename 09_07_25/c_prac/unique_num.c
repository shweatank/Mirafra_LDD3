//o(n)best case and o(n^2)for worst case because if array contains all unique numbers loop never break;
#include<stdio.h>
int main()
{
	int n,res=0,cnt=0;
	printf("enter n value:\n");
	scanf("%d",&n);
	int arr[n];
	for(int i=0;i<n;i++)
	{
		scanf("%d",&arr[i]);
	}
	for(int i=0;i<n;i++)
	{
		cnt=0;
          for(int j=0;j<n;j++)
	  {
		  if(arr[i]==arr[j] && j<i)
			  break;
		  if(arr[i]==arr[j])
		  cnt++;
	  }
	  if(cnt==1)
		  printf("%d ",arr[i]);
	}

}
