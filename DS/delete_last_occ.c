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

st *delete_last_occ(st *head,int d)
{
	st *temp=head,*t,*prev=NULL;
	if(head==NULL)
		return NULL;
	else if(head->link==NULL && head->data==d)
	{
		free(head);
		head=NULL;
		return head;
	}
	else
	{
		while(temp->link)
		{
			if(temp->link->data==d)
			{
				prev=temp;
				t=temp->link;
			}
			temp=temp->link;
		}
		if(prev==NULL && head->data==d)
		{
			free(head);
			head=NULL;
			return head;
		}
		else
		{
			prev->link=t->link;
			free(t);
			return head;
		}
	}
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
	head=delete_last_occ(head,20);
	print(head);
}
