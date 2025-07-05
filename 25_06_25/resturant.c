#include<stdio.h>
#define pizza 250
#define burger 150
float bill;
struct command
{
	void(*order_pizza)(int);
	void(*order_burger)(int);
	float(*view_bill)(void);
	void(*cancelorder)(void);
};
void order_pizza(int count)
{
	printf("----ordered pizza--\n");
	bill+=count*pizza;
}
void order_burger(int count)
{
	printf("----ordered burger---\n");
	bill+=count*burger;
}
float view_bill(void)
{
	printf("----total bill---\n");
	return bill;
}
void cancel(void)
{
	bill=0;
}
struct command a={
	.order_pizza=order_pizza,
	.order_burger=order_burger,
	.view_bill=view_bill,
	.cancelorder=cancel,
};
int main()
{
	char ch;
	float pbill;
	int count=0;
	printf("welcome to c resturant\n");
	printf("choose from the following\n");
//	printf("-->pizza\n-->burger\n-->view bill\n-->cancel order\n");
//	scanf("%c",&ch);
	while(1)
	{
		printf("-->pizza\n-->burger\n-->view bill\n-->cancel order\n");
                scanf(" %c",&ch);

	switch(ch)
	{

		case '1':printf("enter noof pizzas\n");
			 scanf("%d",&count);
			 a.order_pizza(count);
			 break;
	        case '2':printf("entre the noof burgers\n");
			 scanf("%d",&count);
			 a.order_burger(count);
			 break;
		case '3':pbill=a.view_bill();
			 printf("total amount :%f\n",pbill);
			 break;

		case '4':a.cancelorder();
			 printf("order cancelled\n");
			 printf("thanks for visiting our resturant\n");
			 exit(0);
	}
	}
}





