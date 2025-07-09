#include<stdio.h>
#include<stdlib.h>
struct node
{
	int data;
	struct node *next;
	struct node *prev;
};
struct node *head=NULL,*tail=NULL;
void create(void);
void display(void);
void ins_at_beg(int);
void ins_at_end(int);
void ins_at_pos(int);
void del_from_beg(void);
void del_from_end(void);
void del_from_pos(void);
int length(void);
void main()
{
	int ch=0,info=0;
	while(1)
	{
		printf("1.Create\n");
		printf("2.Insert at beginning\n");
		printf("3.Insert at ending\n");
		printf("4.Insert at specific position\n");
		printf("5.Delete from beginning\n");
		printf("6.Delete from ending\n");
		printf("7.Delete from specific position\n");
		printf("8.Display\n");
		printf("9.Exit\n");
		printf("Enter your choice ");
		scanf("%d",&ch);
		switch(ch)
		{
			case 1:
				create();
				break;
			case 2:
				printf("Enter your data ");
				scanf("%d",&info);
				ins_at_beg(info);
				break;
			case 3:
				printf("Enter your data ");
				scanf("%d",&info);
				ins_at_end(info);
				break;
			case 4:
				printf("Enter your data ");
				scanf("%d",&info);
				ins_at_pos(info);
				break;
			case 5:
				del_from_beg();
				break;
			case 6:
				del_from_end();
				break;
			case 7:
				del_from_pos();
				break;
			case 8:
				display();
				break;
			case 9:
				printf("Exitted\n");
				exit(0);
			default:
				printf("Invalid Input\n");
		}
	}
}
void create()
{
	struct node *newnode=NULL;
	int info=0,ch=1;
	while(ch)
	{
		newnode=(struct node *)malloc(sizeof(struct node));
		if(newnode==NULL)
		{
			printf("Malloc error\n");
			return;
		}
		printf("Enter your data ");
		scanf("%d",&info);
		newnode->data=info;
		newnode->prev=newnode;
		newnode->next=newnode;
		if(head==NULL)
		{
			head=tail=newnode;
		}
		else
		{
			tail->next=newnode;
			newnode->prev=tail;
			tail=newnode;
			tail->next=head;
			head->prev=tail;
		}
		printf("Do you want to continue ");
		scanf("%d",&ch);
	}
}
void ins_at_beg(int info)
{
	struct node *newnode=(struct node *)malloc(sizeof(struct node));
	if(newnode==NULL)
	{
		printf("Malloc error\n");
		return;
	}
	newnode->data=info;
	newnode->prev=newnode;
	newnode->next=newnode;
	if(head==NULL)
	{
		head=tail=newnode;
		return;
	}
	head->prev=newnode;
	newnode->next=head;
	newnode->prev=tail;
	head=newnode;
	tail->next=head;
}

void ins_at_end(int info)
{
	struct node *newnode=(struct node *)malloc(sizeof(struct node));
	if(newnode==NULL)
	{
		printf("Malloc error\n");
		return;
	}
	newnode->data=info;
	newnode->prev=newnode;
	newnode->next=newnode;
	if(head==NULL)
	{
		head=tail=newnode;
		return;
	}
	newnode->prev=tail;
	tail->next=newnode;
	tail=newnode;
	tail->next=head;
	head->prev=tail;
}

void ins_at_pos(int info)
{
	int pos=0,i=1,l=0;
	struct node *newnode=NULL,*temp=head;
	printf("Enter your position ");
	scanf("%d",&pos);
	l=length();
	if(pos<=0||pos>l)
	{
		printf("Invalid position\n");
		return;
	}
	newnode=(struct node *)malloc(sizeof(struct node));
	if(newnode==NULL)
	{
		printf("Malloc error\n");
		return;
	}
	newnode->data=info;
	newnode->prev=newnode;
	newnode->next=newnode;
	while(i<pos-1)
	{
		temp=temp->next;
		i++;
	}
	newnode->prev=temp;
	newnode->next=temp->next;
	temp->next=newnode;
	newnode->next->prev=newnode;
}

void del_from_beg()
{
	struct node *temp=head;
	if(head==tail)
	{
		head=tail=NULL;
		free(temp);
		return;
	}
	head=head->next;
	head->prev=tail;
	tail->next=head;
	free(temp);
}

void del_from_end()
{
	struct node *temp=tail;
	if(head==tail)
	{
		head=tail=NULL;
		free(temp);
		return;
	}
	tail=tail->prev;
	tail->next=head;
	head->prev=tail;
	free(temp);
}

void del_from_pos()
{
	struct node *temp=head;
	int pos=0,i=1,l=0;
	printf("Enter your position ");
	scanf("%d",&pos);
	l=length();
	if(pos<=0||pos>l)
	{
		printf("Invalid position\n");
		return;
	}
	while(i<pos)
	{
		temp=temp->next;
		i++;
	}
	temp->prev->next=temp->next;
	temp->next->prev=temp->prev;
	free(temp);
}

int length()
{
	struct node *temp=head;
	int count=0;
	while(temp->next!=head)
	{
		count++;
		temp=temp->next;
	}
	count++;
	return count;
}

void display()
{
	struct node *temp=head;
	if(temp==NULL)
	{
		printf("List is empty\n");
		return;
	}
	while(temp->next!=head)
	{
		printf("%d ",temp->data);
		temp=temp->next;
	}
	printf("%d\n",temp->data);
}

