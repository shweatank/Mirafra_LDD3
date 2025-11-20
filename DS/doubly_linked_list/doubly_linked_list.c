#include<stdio.h>
#include<stdlib.h>
typedef struct s
{
	int data;
	struct s *prev;
	struct s *next;
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
	nu->prev=NULL;
	nu->next=NULL;
	return nu;
}
void forward_print(st *head)
{
	if(head==NULL)
	{
		printf("list is empty\n");
		return;
	}
	printf("forward:");
	while(head)
	{
		printf("%d ",head->data);
		head=head->next;
	}
	printf("\n");
}
void reverse_print(st *tail)
{
	if(tail==NULL)
	{
		printf("list is empty\n");
		return;
	}
	printf("reverse:");
	while(tail)
	{
		printf("%d ",tail->data);
		tail=tail->prev;
	}
	printf("\n");
}
int main()
{
	int n,val;
	st *head=NULL,*tail=NULL;
	printf("enter n value:\n");
	scanf("%d",&n);
	for(int i=0;i<n;i++)
	{
		printf("enter val:\n");
		scanf("%d",&val);
		st *nu=create_node(val);
		if(head==NULL)
			head=tail=nu;
		else
		{
			tail->next=nu;
			nu->prev=tail;
			tail=nu;
		}
	}
	forward_print(head);
	reverse_print(tail);
}
