#include<stdio.h>
#include<stdlib.h>
#define N 5
int stack[N];
int top=-1;
void push(void);
void pop(void);
void peek(void);
void display(void);
void main()
{
	int ch=0;
	while(1)
	{
		printf("1.Push\n");
		printf("2.Pop\n");
		printf("3.Peek\n");
		printf("4.Display\n");
		printf("5.Exit\n");
		printf("Enter your choice ");
		scanf("%d",&ch);
		switch(ch)
		{
			case 1: 
				push();
				break;
			case 2: 
				pop();
				break;
			case 3: 
				peek();
				break;
			case 4: 
				display();
				break;
			case 5: 
				printf("Exitted\n");
				exit(0);
		}
	}
}

void push()
{
	if(top==N-1)
	{
		printf("Overflow condition\n");
		return;
	}
	int data=0;
	printf("Enter your data ");
	scanf("%d",&data);
	top++;
	stack[top]=data;
	
}

void pop()
{
	if(top==-1)
	{
		printf("Underflow condition\n");
		return;
	}
	top--;
}

void peek()
{
	if(top==-1)
	{
		printf("Underflow condition\n");
		return;
	}
	printf("%d\n",stack[top]);
}

void display()
{
	if(top==-1)
	{
		printf("Underflow condition\n");
		return;
	}
	int i=0;
	for(i=top;i>=0;i--)
	{
		printf("%d ",stack[i]);
	}
	printf("\n");
}

