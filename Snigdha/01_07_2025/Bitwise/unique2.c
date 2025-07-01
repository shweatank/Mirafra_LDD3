#include<stdio.h>


int main()
{
	int arr[10]={9,3,1,1,9,3,7,2};

	int xor_all=0;
	for(int i=0;i<10;i++)
		xor_all^=arr[i];
	int set_bit = xor_all & -xor_all;
	int x=0,y=0;

	for(int i=0;i<10;i++)
	{
		if(arr[i] & set_bit)
			x^=arr[i];
		else
			y^=arr[i];
	}
	printf("%d %d\n",x,y);
}
