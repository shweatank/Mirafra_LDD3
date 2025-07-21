#include<stdio.h>
void main()
{
	int size,arr[32],i,j,visited[32]={0};
	printf("Enter size of an array ");
	scanf("%d",&size);
	printf("Enter elements in an array ");
	for(i=0;i<size;i++)
	{
		scanf("%d",&arr[i]);
	}
	/*for(i=0;i<size;i++)
	{
		if(visited[i]==0)
		{
			int count=1;
			for(j=i+1;j<size;j++)
			{
				if(arr[i]==arr[j])
				{
					count++;
					visited[j]=1;
				}
			}	
			printf("%d : %d\n",arr[i],count);
		}
	}*/
	for(i=0;i<size;i++)
	{
		visited[arr[i]]++;
	}
	for(i=0;i<size;i++)
	{
		if(visited[i]>0)
			printf("%d:%d\n",i,visited[i]);
	}
	printf("\n");
}
