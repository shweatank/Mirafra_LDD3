//Find the only non-repeating element in an array where every element repeats twice
//(Hint: Use XOR)
#include<stdio.h>

int main()
{
	int a[10];
	for(int i=0;i<10;i++)
	{
		printf("a[%d]",i);
		scanf("%d",(a+i));
	}
	for(int i=0;i<10;i++)
	{
		int count=0;
		for(int j=0;j<10;j++)
		{
			if((a[i]^a[j])==0)
				count++;
		}
		printf("count of %d is %d\n",a[i],count);
		if(count==1)
		{
			printf("non repeating number is %d\n",a[i]);
			break;
		}
	}
}
