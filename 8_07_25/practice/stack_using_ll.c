#include<stdio.h>
#include<stdlib.h>
struct node 
{
	int data;
	struct node *next;
};
struct node *top=NULL;
void push(void);
void pop(void);
void display(void);
void peek(void);
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
	int info=0;
	struct node *newnode=(struct node *)malloc(sizeof(struct node));
	if(newnode==NULL)
	{
		printf("Malloc error\n");
		return;
	}
	printf("Enter your data ");
	scanf("%d",&info);
	newnode->data=info;
	newnode->next=NULL;
	if(top==NULL)
	{
		top=newnode;
		return;
	}
	newnode->next=top;
	top=newnode;
}

void pop()
{
	struct node *temp=top;
	if(top==NULL)
	{
		printf("stack is empty\n");
		return;
	}
	top=top->next;
	free(temp);
}

void peek()
{
	if(top==NULL)
	{
		printf("stack is empty\n");
		return;
	}
	printf("%d\n",top->data);
}

void display()
{
	struct node *temp=top;
	if(temp==NULL)
	{
		printf("stack is empty\n");
		return;
	}
	while(temp!=NULL)
	{
		printf("%d ",temp->data);
		temp=temp->next;
	}
	printf("\n");
}

