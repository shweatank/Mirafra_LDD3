//clear a bit in hexa
#include<stdio.h>
int main()
{
	short int num=0x1234;
	int i,j,temp1,temp2;
	printf("Enter i and j :\n");
	scanf("%d%d",&i,&j);

	int mask=~(((1<<(j-i+1))-1)<<i);
	num=mask&num;
	printf("num=ox%0X\n",num);

}
        

