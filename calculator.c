//24 June 2025
//Program for basic calculator using function pointers as structure elements
#include<stdio.h>
#include<stdlib.h>
typedef int(*fptr)(int,int);
typedef struct operations
{
	fptr add;
	fptr sub;
	fptr mul;
	fptr div;
}calculator;
int addition(int a,int b)
{
	return a+b;
}
int subtraction(int a,int b)
{
	return a-b;
}
int multiplication(int a,int b)
{
	return a*b;
}
int division(int a,int b)
{
	return a/b;
}
void main()
{
	calculator calcu={.add=addition,.sub=subtraction,.mul=multiplication,.div=division};
	//calculator *calcu=(calculator *)malloc(sizeof(calculator));
	int a=10,b=5;
	/*calcu->add=addition;
	calcu->sub=subtraction;
	calcu->mul=multiplication;
	calcu->div=division;*/
	/*calculator *calcu=&c;
	printf("Add= %d\n",(*(calcu->add))(a,b));
	printf("Sub= %d\n",(*(calcu->sub))(a,b));
	printf("Mul= %d\n",(*(calcu->mul))(a,b));
	printf("Div= %d\n",(*(calcu->div))(a,b));*/
	printf("Add=%d\n",calcu.add(a,b));
	printf("Sub=%d\n",calcu.sub(a,b));
	printf("Mul=%d\n",calcu.mul(a,b));
	printf("Div=%d\n",calcu.div(a,b));

}

