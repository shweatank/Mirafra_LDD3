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
ST * remove_middle(ST *head)
{
	if(head==NULL)
		return NULL;
	else if(head->next==NULL)
	{
		free(head);
		head=NULL;
		return head;
	}
	ST *fast,*slow,*temp=NULL;
        fast=slow=head;
	while(fast && fast->next)
	{
		fast=fast->next->next;
		temp=slow;
		slow=slow->next;
	}
	temp->next=slow->next;
	return head;

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
	head->next->next->next->next->next->next=create_node(100);
	print_list(head);
	head=remove_middle(head);
	print_list(head);
}
