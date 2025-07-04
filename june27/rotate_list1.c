
#include<stdio.h>
#include<stdlib.h>
typedef struct s
{
	int data;
	struct s *link;
}st;
st *create_node(int d)
{
	st *nu=NULL;
	nu=calloc(1,sizeof(st));
	if(nu==NULL)
	{
		printf("memory allocation failed\n");
		return 0;
	}
	nu->data=d;
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
st *rotate_list(st *head,int n)
{
	st *temp=head;
	if(n==0 || head==NULL)
		return head;
	int len=1;
	while(temp->link)
	{
		len++;
		temp=temp->link;
	}
	n=n%len;
	for(int i=0;i<n;i++)
	{
		temp=head;
		while(temp->link)
		{
			temp=temp->link;
		}
		temp->link=head;
		head=head->link;
		temp->link->link=NULL;
	}
	return head;
}
int main()
{
	st *head=NULL;
	head=create_node(10);
	head->link=create_node(20);
	head->link->link=create_node(30);
	head->link->link->link=create_node(40);
	head->link->link->link->link=create_node(50);

	print(head);
	head=rotate_list(head,2);
	print(head);
}
