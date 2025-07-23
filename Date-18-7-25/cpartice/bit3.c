//swap a nibbles 
#include<stdio.h>
int main()
{
	int num=0x12345678;

	int mask1=num>>24;
	int mask2=(num>>16)<<24;
	int mask3=(num>>8)<<24;
	int mask4=num<<24;
        printf("masl1=0x%0x\n",mask1);
        printf("masl1=0x%0x\n",mask2);
        printf("masl1=0x%0x\n",mask3);
        printf("masl1=0x%0x\n",mask4);
	printf("num=0x%x\n",mask4|mask3>>8|mask2>>16|mask1);
}

