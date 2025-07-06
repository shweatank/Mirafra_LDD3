#include<stdio.h>
int hcf(int a,int b)
{
	static int c,f;
	if(f==0)
	{
		c=(a>b)?b:a;
		f=1;
	}
	if(c>0)
	{
		if(a%c==0 && b%c==0)
			return c;
		c--;
		return hcf(a,b);
	}
	else
		return 0;
}
/*
int hcf(int a, int b) {
    if (b == 0)
        return a;
    return hcf(b, a % b);
}

*/
void main()
{
	int num1,num2;
	printf("Enter the numbers : ");
	scanf("%d%d",&num1,&num2);
	printf("HCF = %d\n",hcf(num1,num2));
}
