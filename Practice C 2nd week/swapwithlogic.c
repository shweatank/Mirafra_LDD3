#include<stdio.h>
void main()
{
	int a,b;
	printf("Enter the 2 numebrs : ");
	scanf("%d%d",&a,&b);
	/*a=a^b;
	b=a^b;
	a=a^b;*/
	a^=b^=a^=b;
	//a=a^b^(b=a);
	printf("%d  %d  \n",a,b);
}
