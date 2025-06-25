#include<stdio.h>
#include<stdlib.h>

typedef struct
{
  int a;
  int b;
  void (*fp1)(int a,int b);
  void (*fp2)(int a,int b);
  void (*fp3)(int a, int b);

}Simple;

void Add(int x, int y)
{
	printf("Addition is %d\n ",(x+y));
	return;
}

void Sub(int x, int y)
{
	printf("Subtraction is %d\n ",(x-y));
	return;
}

void Cal(Simple *s)
{
	printf("Addition CAL :-\n");
	s->fp1(s->a,s->b);
	 
	printf("Subtraction CAL :-");
	s->fp2(s->a,s->b);
}
int main()
{
	//int m =1;
	//int n =1;
	Simple s;
	s.a = 10;
	s.b = 20;
	s.fp1 = Add;
	s.fp2=Sub;
	s.fp3=NULL;

	s.fp1(s.a,s.b);
	s.fp2(s.a,s.b);
	Cal(&s);

	return 0;
}
