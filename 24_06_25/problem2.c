#include<stdio.h>
#include<stdlib.h>
struct fun
{
	int (*fptr)(int,int);
	int (*fptr1)(int,int);
	int (*fptr2)(int,int);
	int (*fptr3)(int,int);
};
int mul(int a,int b)
{
	return a*b;
}
int divi(int a,int b)
{
	if(b<=0)
	{
		return -1;
	}
	return a/b;
}
int add(int a,int b)
{
	return a+b;
}
int sub(int a,int b)
{
	return a-b;
}
void main()
{
//	struct fun *s=(struct fun *)malloc(sizeof(struct fun));//through structure pointer
	int x=0,y=0;
	printf("Enter two numbers ");
	scanf("%d %d",&x,&y);
//	s->fptr=add;
//	s->fptr1=sub;
//	s->fptr2=mul;
//	s->fptr3=divi;
//	struct fun s1={add,sub,mul,divi};//through structure variable
	struct fun s={
		.fptr=add,
		.fptr1=sub,
		.fptr2=mul,
		.fptr3=divi
	};
//	printf("Add: %d\n",s->fptr(x,y));
//	printf("Sub: %d\n",s->fptr1(x,y));
//	printf("Mul: %d\n",s->fptr2(x,y));
//	printf("Div: %d\n",s->fptr3(x,y));
	printf("Add: %d\n",s.fptr(x,y));
	printf("Sub: %d\n",s.fptr1(x,y));
	printf("Mul: %d\n",s.fptr2(x,y));
	printf("Div: %d\n",s.fptr3(x,y));
}


