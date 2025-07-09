#include<stdio.h>
#include<stdlib.h>
struct node
{
	int data;
	struct node *next;
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
		printf("5.Delete from the beginning\n");
		printf("6.Delete from the ending\n");
		printf("7.Delete from the specific position\n");
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
				printf("Exit\n");
				exit(0);
			default:
				printf("Invalid input\n");
		}
	}
}
void create()
{
	struct node *newnode=NULL;
	int info=0,choice=1;
	while(choice)
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
		newnode->next=newnode;
		if(head==NULL)
		{
			head=tail=newnode;
		}
		else
		{
			tail->next=newnode;
			newnode->next=head;
			tail=newnode;
		}
		printf("Do you want to continue ");
		scanf("%d",&choice);
	}
}

void ins_at_beg(int info)
{
	struct node *newnode=NULL;
	newnode=(struct node *)malloc(sizeof(struct node));
	if(newnode==NULL)
	{
		printf("Malloc error\n");
		return;
	}
	newnode->data=info;
	newnode->next=newnode;
	if(head==NULL)
	{
		head=tail=newnode;
		return;
	}
	newnode->next=head;
	head=newnode;
	tail->next=head;
}

void ins_at_end(int info)
{
	struct node *newnode=NULL;
	newnode=(struct node *)malloc(sizeof(struct node));
	if(newnode==NULL)
	{
		printf("Malloc error\n");
		return;
	}
	newnode->data=info;
	newnode->next=newnode;
	if(head==NULL)
	{
		head=tail=newnode;
		return;
	}
	tail->next=newnode;
	tail=newnode;
	newnode->next=head;
}

void ins_at_pos(int info)
{
	struct node *newnode=NULL,*temp=head;
	int i=1,pos=0,l=0;
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
	newnode->next=newnode;
	while(i<pos-1)
	{
		temp=temp->next;
		i++;
	}
	newnode->next=temp->next;
	temp->next=newnode;
}

void del_from_beg()
{
	struct node *temp=head;
	if(head->next==head)
	{
		head=tail=NULL;
		free(temp);
		return;
	}
	head=head->next;
	tail->next=head;
	free(temp);
}

void del_from_end()
{
	struct node *temp=head,*prev=NULL;
	if(head->next==head)
	{
		head=tail=NULL;
		free(temp);
		return;
	}
	while(temp->next!=head)
	{
		prev=temp;
		temp=temp->next;
	}
	tail=prev;
	tail->next=head;
	free(temp);
}

void del_from_pos()
{
	struct node *temp=head,*prev=NULL;
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
		prev=temp;
		temp=temp->next;
		i++;
	}
	prev->next=temp->next;
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
