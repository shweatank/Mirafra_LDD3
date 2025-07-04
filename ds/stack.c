#include<stdio.h>
#include<stdlib.h>
typedef struct st
{
	int data;
	struct st *next;
}ST;
ST *top=NULL;
void push(int data)
{
	ST *nu;
	nu=(ST *)malloc(sizeof(ST));
	if(nu==NULL)
	{
		printf("memory allocation failed\n");
		return ;
	}
	nu->data=data;
	nu->next=top;
	top=nu;
}
void pop()
{
	if(top==NULL)
	{
		printf("stack is empty\n");
		return;
	}
	ST *temp=top;
	printf("pop:%d\n",top->data);
	top=top->next;
	free(temp);
}
void peek()
{
	if(top==NULL)
	{
		printf("stack is empty\n");
		return;
	
	printf("peek:%d\n",top->data);
	}

}
void print_list(ST *head)
{
	if(head==NULL)
	{
		printf("list is empty\n");
		return;
	}
	printf("stack:");
	while(head)
	{
		printf("%d ",head->data);
		head=head->next;
	}
	printf("\n");
}
int main()
{
	push(10);
	push(20);
	push(30);
	print_list(top);
	pop();
	print_list(top);
	peek();
	pop();
	pop();
	print_list(top);
	pop();
}
