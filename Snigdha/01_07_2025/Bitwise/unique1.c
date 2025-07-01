#include<stdio.h>


int main()
{
	int arr[]={5,7,2,5,2};
	int res=0;
	for(int i=0;i<5;i++)
		res^=arr[i];
	printf("%d\n",res);
}
