#include<stdio.h>

typedef struct
{
	int (*Add)(int,int);
	int (*Sub)(int,int);
	int (*Mul)(int,int);
	int (*Div)(int,int);
}cal;

int add(int a,int b)
{
	return a+b;
}
int sub(int a,int b)
{
	return a-b;
}
int mul(int a,int b)
{
	return a*b;
}
int div(int a,int b)
{
	return a/b;
}
int call_back(int a,int b,int (*fp)(int,int))
{
	int result=(*fp)(a,b);
	return result;
}

int main()
{
	int num1,num2,result,op;
	cal cal1={
		.Add=add,
		.Sub=sub,
		.Mul=mul,
		.Div=div};
	printf("enter num1 and num2\n");
	scanf("%d",&num1);
	scanf("%d",&num2);
	while(1)
	{	
		printf("enter the operation to be performed 1.add 2.sub 3.mul 4.div 5.exit\n");
		scanf("%d",&op);
		switch(op)
		{
			case 1:
				result=call_back(num1,num2,cal1.Add);
				printf("result %d\n",result);
				break;
			case 2:result=call_back(num1,num2,cal1.Sub);
                                printf("result %d\n",result);
                                break;
			case 3:result=call_back(num1,num2,cal1.Mul);
                                printf("result %d\n",result);
                                break;
			case 4:
				result=call_back(num1,num2,cal1.Div);
                                printf("result %d\n",result);
                                break;
			case 5:
				break;
			default:
				printf("invalid option\n");
		}
		if(op==5)
			break;
	}


}
