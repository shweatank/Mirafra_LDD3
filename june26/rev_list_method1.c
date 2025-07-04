#include<stdio.h>
#include<stdlib.h>
typedef struct s
{
	int data;
	struct s *link;
}st;
st *create_node(int d)
{
	st *nu=NULL;
	nu=calloc(1,sizeof(st));
	if(nu==NULL)
	{
		printf("memory allocation failed\n");
		return 0;
	}
	nu->data=d;
	nu->link=NULL;
	return nu;
}
void print(st *head)
{
	if(head==NULL)
	{
		printf("list is empty\n");
		return ;
	}
	while(head)
	{
		printf("%d ",head->data);
		head=head->link;
	}
	printf("\n");
}
st *rev_nodes(st *head)
{
	if(head==NULL || head->link==NULL)
		return head;
	st *prev=NULL,*cur=head,*next=head;
	while(cur)
	{
		next=cur->link;
		cur->link=prev;

		prev=cur;
		cur=next;
	}
	return prev;
}
int main()
{
	st *head=NULL;
	head=create_node(10);
	head->link=create_node(20);
	head->link->link=create_node(30);
	head->link->link->link=create_node(40);
	head->link->link->link->link=create_node(50);

	print(head);
	head=rev_nodes(head);
	print(head);
}
