#include<stdio.h>
unsigned  int to_swap(unsigned int num)
{
	int result=0;
	char *src=(char *)&num;
	char *des=(char *)&result;
	for(int i=0,j=3;i<=j;i++)
	{
		des[i]=src[j-i];
	}
	return result;

}
int main()
{
	unsigned int num;
	printf("enter  int:");
	scanf("%x",&num);
	printf("before swap:0x%08x\n",num);
	printf("after swap:0x%08x\n",to_swap(num));
}
