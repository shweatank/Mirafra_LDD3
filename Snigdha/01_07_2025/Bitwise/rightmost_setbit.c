#include<stdio.h>


int main()
{
	int n;
	scanf("%d",&n);

	int res= (n & -n);
	printf("%d\n",res);
}
