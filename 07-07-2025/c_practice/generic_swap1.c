#include<stdio.h>
#include<string.h>
typedef struct s
{
	int i;
	int j;
}st;
void generic_swap(void *a,void *b,size_t size)
{
	char temp[100];
	memcpy(temp,a,size);
	memcpy(a,b,size);
	memcpy(b,temp,size);
}

int main()
{
	int a=10,b=20;
	printf("before swap:a=%d b=%d\n",a,b);
	generic_swap(&a,&b,sizeof(int));
	printf("after swap:a=%d b=%d\n",a,b);

	float f1=1.2,f2=3.4;
	printf("before swap:f1=%f f2=%f\n",f1,f2);
	generic_swap(&f1,&f2,sizeof(float));
	printf("after swap:f1=%f f2=%f\n",f1,f2);

	st var1={2,3,},var2={4,5};
	printf("before swap: var1:%d %d var2:%d %d\n",var1.i,var1.j,var2.i,var2.j);
	generic_swap(&var1,&var2,sizeof(st));
	printf("after swap: var1:%d %d var2:%d %d\n",var1.i,var1.j,var2.i,var2.j);
}

