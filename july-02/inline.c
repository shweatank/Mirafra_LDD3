#include<stdio.h>
 inline void add(int a,int b)
{
	int c=a+b;
	printf("%d",c);

}
int main()
{
	int a=10,b=5;
	add(a,b);
	add(a,b);
	add(a,b);
}
