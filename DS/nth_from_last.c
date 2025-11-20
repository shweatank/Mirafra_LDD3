#include<stdio.h>
#include<stdlib.h>
typedef struct s
{
	int data;
	struct s *link;
}st;
void print(st *head)
{
	if(head==NULL)
	{
		printf("list is empty\n");
		return;
	}
	while(head)
	{
		printf("%d ",head->data);
		head=head->link;
	}
	printf("\n");
}
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
st *nth_from_last(st *head,int n)
{
	if(n<=0)
	{
		printf("invalid n value\n");
		return NULL;
	}
	st *first=head,*second=head;
	for(int i=0;i<n;i++)
	{
		if(first==NULL)
		{
			printf("invalid n value\n");
			return NULL;
		}
		first=first->link;
	}
	while(first)
	{
		first=first->link;
		second=second->link;
	}
	return second;
}
int main()
{
	st *head=NULL,*temp=NULL;
	int n,val;
	printf("enter n:\n");
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
	st *t=nth_from_last(head,2);
	if(t!=NULL)
		printf("nth node from last:%d\n",t->data);
}
