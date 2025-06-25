#include<stdio.h>

typedef struct
{
	int a;
	int b;
	void (*fp1)(int a,int b);
	void (*fp2)(int a,int b);
}Simple;

void Add(int x,int y)
{
	printf("Addition is %d\n",x+y);
}

void Sub(int x,int y)
{
	printf("Subtracion is %d\n",x-y);
}
void Cal(Simple *s)
{

	printf("Addition CAL :- \n");
	s->fp1(s->a,s->b);

	printf("Subtraction CAL :-\n");
	s->fp2(s->a,s->b);
}

int main()
{
	Simple s = {.a=5,.b=2,.fp1=Add,.fp2=Sub};

	s.fp1(s.a,s.b);
	s.fp2(s.a,s.b);

	Cal(&s);
	return 0;
}

