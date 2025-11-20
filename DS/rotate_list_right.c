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
st *rotate_right(st *head,int r)
{
	if(head==NULL || r==0)
		return head;
	st *nu=NULL,*temp=head,*tail=head;
	int len=1;
	while(tail->link)
	{
		len++;
		tail=tail->link;
	}
	r=r%len;
	if(r==0)
		return head;
	for(int i=1;i<len-r;i++)
	{
		temp=temp->link;
	}
	nu=temp->link;
	temp->link=NULL;
	tail->link=head;
	return nu;
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
	head=rotate_right(head,2);
	print(head);
}

