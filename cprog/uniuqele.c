#include<stdio.h>

int unique(int *a,int count)
{
	int c=0;
	for(int i=0;i<count;i++)
	{
		c=0;
		for(int j=0;j<count;j++)
		{
			if((a[i] & a[j])==a[i])
				c++;
		}
		if(c==1)
		{
			return a[i];
		}
	}
	return 0;
}
/*int unique(int *a, int n)
{
    int res = 0;
    for (int i = 0; i < n; i++)
        res ^= a[i];  // XOR cancels out duplicates
    return res;
}*/

int main()
{
	int a[10],count=9;
	for(int i=0;i<count;i++)
	{
		scanf("%d",&a[i]);
	}
	int c=unique(a,count);
	printf("unique :%d\n",c);
	return 0;
}
