#include<stdio.h>
int main()
{
	int num;
	printf("Enter The 4 Digit Number:\n");
	scanf("%d",&num);

	int d1=(num/1000)%10;
	int d2=(num/100)%10;
	int d3=(num/10)%10;
	int d4=num%10;

	int result=d1*1000+d3*100+d2*10+d4;

	printf("swapped=%d\n",result);
}
