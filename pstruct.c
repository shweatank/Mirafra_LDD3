#include<stdio.h>
typedef struct {
    int x,y;
    void (*add)(int,int);
    void (*subtract)(int,int);
    void (*mul)(int,int);
    void (*div)(int,int);
} Calculator;

void addFunc(int a,int b) 
{
	printf("sum:%d\n",a+b);
}
void subFunc(int a,int b)
{
       printf("sub:%d\n",a-b);	
}
void mulFunc(int a,int b)
{
	printf("mul:%d\n",a*b);
}
void divFunc(int a,int b)
{
	printf("div:%d\n",a/b);
}
void cal(int a,int b,void(*op)(int ,int))
{
	//printf("%d\n",op(a,b));
	op(a,b);
}

int main()
{
    Calculator c = { .x =5,.y=2, .add = addFunc, .subtract = subFunc,.mul=mulFunc,.div=divFunc };
    Calculator *cp = &c;

    //cp->add(cp->x,cp->y);    //structure pointer
    //cp->subtract(cp->x,cp->y);
    	cal(cp->x,cp->y,cp->add);  //call back function
	cal(cp->x,cp->y,cp->subtract);
	cal(cp->x,cp->y,cp->mul);
	cal(cp->x,cp->y,cp->div);
}


