#include<stdio.h>

int main()
{

	unsigned int n;
	scanf("%d",&n);
	int d;
	scanf("%d",&d);

	int res=((n<<d) | (n>>(32-d)));
	printf("%d\n",res);
}
