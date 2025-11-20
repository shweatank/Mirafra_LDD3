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
st *merge(st *head1,st *head2)
{
	st *cur1=head1,*cur2=head2,*next1,*next2;
	while(cur1 && cur2)
	{
		next1=cur1->link;
		next2=cur2->link;
		cur2->link=cur1->link;
		cur1->link=cur2;
		cur1=next1;
		cur2=next2;
	}
	return cur2;
}
int main()
{
	st *head1=NULL,*head2=NULL,*temp=NULL;
	int n,val;
	printf("enter n for list1:\n");
	scanf("%d",&n);
	for(int i=0;i<n;i++)
	{
		printf("enter val:\n");
		scanf("%d",&val);
		st *nu=create_node(val);
		if(head1==NULL)
			head1=temp=nu;
		else
		{
			temp->link=nu;
			temp=nu;

		}
	}
	printf("enter n for list2:\n");
	scanf("%d",&n);
	for(int i=0;i<n;i++)
	{
		printf("enter val:\n");
		scanf("%d",&val);
		st *nu=create_node(val);
		if(head2==NULL)
			head2=temp=nu;
		else
		{
			temp->link=nu;
			temp=nu;

		}
	}
	print(head1);
	print(head2);
	st *remaining=merge(head1,head2);
	printf("after merge:\n");
	printf("list1:");
	print(head1);
	printf("list2:");
	print(remaining);
}
