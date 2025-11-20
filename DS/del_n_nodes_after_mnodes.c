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
st *delete_n_after_m(st *head,int n,int m)
{
	st *temp=head,*t,*prev;
	if(n==0 || head==NULL)
		return head;
	if(m==0)
	{
		while(temp)
		{
			temp=head;
			head=head->link;
			free(temp);
		}
		return head;
	}
	temp=head;
	int i,j;
	while(temp)
	{
		i=0;
		while(i<m && temp)
		{
			prev=temp;
			temp=temp->link;
			i++;
		}
		j=0;
		while(j<n && temp)
		{
			t=temp;
			prev->link=temp->link;
			temp=temp->link;
			free(t);
			j++;
		}
	}
	return head;
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
	head=delete_n_after_m(head,2,2);
	print(head);
}
