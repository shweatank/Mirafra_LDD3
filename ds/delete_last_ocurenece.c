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
ST *last_occur_node(ST*head,int data)
{
	int cnt=1;
	if(head==NULL)
		return head;
	ST *temp=head,*cur=NULL,*prev=NULL,*next=NULL;
	while(temp)
	{
		if(temp->data==data)
		{
			prev=cur;
			next=temp->next;
		}
		cur=temp;
		temp=temp->next;
	}
		if(next==NULL)
		{
			printf("data not found\n");
		}
		else if(next==head->next)
		{
			temp=head;
			head=head->next;
			free(temp);
		}
		else
		{
		prev->next=next;
		}
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
	head=last_occur_node(head,10);
	print_list(head);
}
