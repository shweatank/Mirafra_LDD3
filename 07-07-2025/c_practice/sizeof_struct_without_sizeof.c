#include<stdio.h>
typedef struct s
{
	int a;
	char b;
	double c;
}st;

int main()
{
	st var[2];
	char *ptr1=(char *)&var[0];
	char *ptr2=(char *)&var[1];
	int size=ptr2-ptr1;
	printf("size of struct:%d\n",size);
}
