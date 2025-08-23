#include<stdio.h>
int to_count(unsigned int num,int *ptr)
{
	int cnt=0,i=0;
	for(int bit=31;bit>=0;bit--)
	{
		if(num>>bit&1)
		{
			cnt++;
			ptr[i++]=bit;
		}
	}
	return cnt;
}
int main()
{
	unsigned int num;
	int arr[32];
	scanf("%u",&num);
	int cnt=to_count(num,arr);
	printf("set bit count :%d\n",cnt);
	printf("set bit positions:");
	for(int i=0;i<cnt;i++)
		printf("%d ",arr[i]);


}

