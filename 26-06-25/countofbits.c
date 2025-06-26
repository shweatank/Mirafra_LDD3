//26 June 2025
//To count number of set bits for numbers from 1 to n
#include<stdio.h>
#include<stdlib.h>
int * countofbits(int n)
{
	int *arr=(int *)malloc(sizeof(int));
	int i,pos,count;
	for(i=1;i<=n;i++)
	{
		for(count=0,pos=31;pos>=0;pos--)
		{
			if(i>>pos&1)
				count++;
		}
		arr[i-1]=count;
	}
	return arr;
}
void main()
{
	int num,*result;
	printf("Enter the n : ");
	scanf("%d",&num);
	result=countofbits(num);
	printf("REsulting array : ");
	for(int i=0;i<num;i++)
		printf("%d  ",result[i]);
	printf("\n");
}
