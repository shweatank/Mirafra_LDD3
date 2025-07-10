#include<stdio.h>
#include<stdlib.h>
#define N 5
int queue[N];
int front=-1,rear=-1;
void enqueue(void);
void dequeue(void);
void peek(void);
void display(void);
void main()
{
	int ch=0;
	while(1)
	{
		printf("1.Enqueue\n");
		printf("2.Dequeue\n");
		printf("3.Peek\n");
		printf("4.Display\n");
		printf("5.Exit\n");
		printf("Enter your choice ");
		scanf("%d",&ch);
		switch(ch)
		{
			case 1: 
				enqueue();
				break;
			case 2:
				dequeue();
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
			default:
				printf("Invalid input\n");
		}
	}
}

void enqueue()
{
	int info=0;
	if(rear==N-1)
	{
		printf("Overflow condition\n");
		return;
	}
	printf("Enter your data ");
	scanf("%d",&info);
	if(front==-1&&rear==-1)
	{
		front=rear=0;
		queue[rear]=info;
		return;
	}
	rear++;
	queue[rear]=info;
}

void dequeue()
{
	if(front==-1 && rear==-1)
	{
		printf("Underflow condition\n");
		return;
	}
	front++;
}

void peek()
{
	if(front==-1&&rear==-1)
	{
		printf("Underflow condition\n");
		return;
	}
	printf("%d\n",queue[rear]);
}

void display()
{
	int i=0;
	for(i=front;i<=rear;i++)
	{
		printf("%d ",queue[i]);
	}
	printf("\n");
}
