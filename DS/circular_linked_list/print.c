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
		return;
	}
	st *cur=head;
	do{
		printf("%d ",cur->data);
		cur=cur->link;
	}while(cur!=head);
	printf("\n");
}
int main()
{
	int n,val;
	st *head=NULL,*t=NULL;
	printf("enter n value:\n");
	scanf("%d",&n);
	for(int i=0;i<n;i++)
	{
		printf("enter val:\n");
		scanf("%d",&val);
		st *nu=create_node(val);
		if(head==NULL)
		{
			head=nu;
			head->link=head;
		}
		else
		{
			t=head;
			while(t->link!=head)
				t=t->link;
			t->link=nu;
			nu->link=head;
		}
	}
	print(head);
}

