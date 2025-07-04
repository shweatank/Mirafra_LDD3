#include<stdio.h>
#include<stdlib.h>
typedef struct st
{
	int data;
	struct st *next;
}ST;
ST *create_node(int data)
{
	ST *nu;
	nu=(ST *)malloc(sizeof(ST));
	if(nu==NULL)
	{
		printf("mmeory allocation failed\n");
		return 0;
	}
	nu->data=data;
	nu->next=NULL;
	return nu;
}
ST * remove_loop(ST *head)
{
	if(head==NULL)
		return 0;
	ST *fast,*slow;
	fast=slow=head;
	while(fast && fast->next)
	{
		fast=fast->next->next;
		slow=slow->next;
		if(slow==fast)
		break;
	}
	if(slow==fast)
	{
		slow=head;
		if(fast==slow)
		{
	         while(fast->next!=slow)
			 fast=fast->next;
		}
		else
		{
			while(fast->next!=slow->next)
			{
				fast=fast->next;
				slow=slow->next;
			}
		}
	fast->next=NULL;
	}
	return 0;
}
void print_list(ST *head)
{
	if(head==NULL)
	{
		printf("list is empty\n");
		return;
	}
	while(head)
	{
		printf("%d ",head->data);
		head=head->next;
	}
	printf("\n");
}
int main()
{
	ST *head=create_node(10);
	head->next=create_node(20);
	head->next->next=create_node(50);
	head->next->next->next=create_node(30);
	head->next->next->next->next=create_node(40);
	head->next->next->next->next->next=create_node(90);
	head->next->next->next->next->next->next=head->next;
         head=remove_loop(head);
	 print_list(head);
}
