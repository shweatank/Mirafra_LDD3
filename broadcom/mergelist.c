#include<stdio.h>
#include<stdlib.h>
typedef struct s
{
	int data;
	struct s *link;
}st;
st *head1,*head2;
st *create_node(int val)
{
	st *nu=calloc(1,sizeof(st));
	if(nu==NULL)
		return NULL;
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
	st *temp=head;
	while(temp)
	{
		printf("%d ",temp->data);
		temp=temp->link;
	}
	printf("\n");
}
st *merge(st *head1,st *head2)
{
	st *temp=NULL,*tail=NULL;
	st *h1=head1,*h2=head2;
	st *res=NULL;
	while(h1 && h2)
	{
		if(h1->data <= h2->data)
		{
			temp=h1;
			h1=h1->link;
		}
		else
		{
			temp=h2;
			h2=h2->link;
		}
		if(res==NULL)
		{
			res=temp;
			tail=temp;
		}
		else
		{
			tail->link=temp;
			tail=tail->link;
		}
	}
	if(h1)
		tail->link=h1;
	else
		tail->link=h2;
	return res;
}

int main()
{
	int n1,n2;
	st *temp;
	printf("enter n1,n2 val:\n");
	scanf("%d%d",&n1,&n2);
	for(int i=0;i<n1;i++)
	{
		int val;
		printf("enter val to insert:\n");
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
	for(int i=0;i<n2;i++)
	{
		int val;
		printf("enter val to insert:\n");
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
	printf("list1:");
	print(head1);
	printf("list2:");
	print(head2);
	st *res=merge(head1,head2);
	printf("res:");
	print(res);
}

