#include<stdio.h>
#include<stdlib.h>
typedef struct st
{
	int data;
	struct st *next;
}ST;
ST *create_node(int data)
{
	ST *nu;
	nu=(ST *)malloc(sizeof(ST));
	if(nu==NULL)
	{
		printf("mmeory allocation failed\n");
		return 0;
	}
	nu->data=data;
	nu->next=NULL;
	return nu;
}
ST * rotate(ST*head,int n)
{
	int cnt=0;
	if(head==NULL || n<=0)
		return head;
	ST *temp=head;
	while(temp->next)
	{
		cnt++;
		temp=temp->next;
	}
	n=n%cnt;
	if(n==0)
		return head;
         cur=head;
	 for(int i=0;i<n;i++)
	 {
            prev=cur;
            cur=cur->next;
	 }
	 temp->next=head;
	 prev->next=NULL;



	 return head;
}
void print_list(ST *head)
{
	if(head==NULL)
	{
		printf("list is empty\n");
		return;
	}
	while(head)
	{
		printf("%d ",head->data);
		head=head->next;
	}
	printf("\n");
}
int main()
{
	ST *head=create_node(10);
	head->next=create_node(20);
	head->next->next=create_node(50);
	head->next->next->next=create_node(30);
	head->next->next->next->next=create_node(40);
	head->next->next->next->next->next=create_node(90);
	head->next->next->next->next->next->next=create_node(100);
	print_list(head);
	head=rotate(head,2);
	print_list(head);
}
