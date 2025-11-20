#include<stdio.h>
#include<stdlib.h>
typedef struct s
{
	int data;
	struct s *link;
}st;
st *create_node(int val)
{
	st *nu=calloc(1,sizeof(st));
	if(nu==NULL)
	{
		printf("memory allocation failed\n");
		return NULL;
	}
	nu->data=val;
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
st *print_middle(st *head)
{
	if(head==NULL || head->link==NULL)
		return head;
	st *slow=head,*fast=head;
	while(fast && fast->link)
	{
		slow=slow->link;
		fast=fast->link->link;
	}
	return slow;
}
int main()
{
	st *head=NULL,*temp=NULL;
	int n,val;
	printf("enter n value:\n");
	scanf("%d",&n);
	for(int i=0;i<n;i++)
	{
		printf("enter val:\n");
		scanf("%d",&val);
		st *nu=create_node(val);
		if(head==NULL)
			head=temp=nu;
		else
		{
			temp->link=nu;
			temp=nu;
		}
	}
	print(head);
	st *t=print_middle(head);
	if(t!=NULL)
		printf("middle node:%d\n",t->data);
}

