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
st *reverse_list(st *head)
{
	if(head==NULL || head->link==NULL)
		return head;
	st *prev=NULL,*cur=head,*next=NULL;
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
	head=reverse_list(head);
	print(head);
}
