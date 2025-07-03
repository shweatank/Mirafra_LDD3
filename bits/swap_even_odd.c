#include<stdio.h>
void printbinary(int num)
{
        int bit=31;
        while(bit>=0)
        {
                if((num>>bit)&1)
                {
                        printf("1");
                }
                else
                {
                        printf("0");
                }
                bit--;
        }
	printf("\n");
}
int swap(int num)
{
	return (((num & 0xAAAAAAAA)>>1) | ((num & 0x55555555)<<1));
}
int main()
{
	int num,n;
	printf("entre the num:\n");
	scanf("%d",&num);
	printbinary(num);
	n=swap(num);
	printbinary(n);
}
