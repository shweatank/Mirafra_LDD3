#include<stdio.h>
int to_extract(unsigned int num,int p1,int p2)
{
	unsigned int mask;
	if(p1==0 && p2==31)
		mask=0xffffffff;
	else
	mask=((1u<<(p2-p1+1))-1)<<p1;

	return (mask&num)>>p1;
}
int main()
{
	unsigned int res,num;
	int pos1=4,pos2=10;
	scanf("%u",&num);
	scanf("%d%d",&pos1,&pos2);
//	to_print(num);
	res=to_extract(num,pos1,pos2);
	printf("%d",res);
}
