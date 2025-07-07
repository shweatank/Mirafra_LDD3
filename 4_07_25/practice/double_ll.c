#include<stdio.h>
#include<stdlib.h>
struct node
{
	int data;
	struct node *prev;
	struct node *next;
};
struct node *head=NULL,*tail=NULL;
void create(void);
void ins_at_beg(int);
int length(void);
void del_from_beg(void);
void del_from_end(void);
void del_from_pos(void);
void ins_at_end(int);
void ins_at_pos(int);
void reverse(void);
void display(void);
void main()
{
	int ch,info;
	while(1)
	{
		printf("1.Create\n");
		printf("2.Insert at beginning\n");
		printf("3.Insert at ending\n");
		printf("4.Insert at specific position\n");
		printf("5.Delete from beginning\n");
		printf("6.Delete from ending\n");
		printf("7.Delete from specific position\n");
		printf("8.Reverse\n");
		printf("9.Display\n");
		printf("10.Exit\n");
		printf("Enter your choice ");
		scanf("%d",&ch);
		switch(ch)
		{
			case 1: create();
				break;
			case 2: printf("Enter your data ");
				scanf("%d",&info);
				ins_at_beg(info);
				break;
			case 3: printf("Enter your data ");
				scanf("%d",&info);
				ins_at_end(info);
				break;
			case 4: printf("Enter your data ");
				scanf("%d",&info);
				ins_at_pos(info);
				break;
			case 5: del_from_beg();
				break;
			case 6: del_from_end();
				break;
			case 7: del_from_pos();
				break;
			case 8: reverse();
				break;
			case 9: display();
				break;
			case 10: printf("Exit\n");
				exit(0);
			default: printf("Invalid input\n");
		}
	}
}
void create()
{
	struct node *newnode=NULL;
	int choice=1,info;
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
		newnode->prev=NULL;
		newnode->next=NULL;
		if(head==NULL)
		{
			head=tail=newnode;
		}
		else
		{
			tail->next=newnode;
			newnode->prev=tail;
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
	newnode->next=NULL;
	newnode->prev=NULL;
	if(head==NULL &&tail==NULL)
	{
		head=tail=newnode;
		return;
	}
	head->prev=newnode;
	newnode->next=head;
	head=newnode;
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
	newnode->next=NULL;
	newnode->prev=NULL;
	if(head==NULL && tail==NULL)
	{
		head=tail=newnode;
		return;
	}
	newnode->prev=tail;
	tail->next=newnode;
	tail=newnode;
}

int length()
{
	struct node *temp=head;
	int count=0;
	while(temp!=NULL)
	{
		count++;
		temp=temp->next;
	}
	return count;
}

void ins_at_pos(int info)
{
	struct node *newnode=NULL,*temp=head;
	int pos=0,i=1,l=0;
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
	newnode->next=NULL;
	newnode->prev=NULL;
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
	if(temp==NULL)
	{
		printf("List is empty\n");
		return;
	}
	if(head==tail)
	{
		head=tail=NULL;
		free(temp);
		return;
	}
	head=head->next;
	head->prev=NULL;
	free(temp);
}

void del_from_end()
{
	struct node *temp=tail;
	if(head==NULL)
	{
		printf("List is empty\n");
		return;
	}
	if(head==tail)
	{
		head=tail=NULL;
		free(temp);
		return;
	}
	tail=tail->prev;
	tail->next=NULL;
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

void reverse()
{
	struct node *previous=NULL,*current=head,*nextnode=head;
	while(nextnode!=NULL)
	{
		nextnode=current->next;
		current->prev=nextnode;
		current->next=previous;
		previous=current;
		current=nextnode;
	}
	tail=head;
	head=previous;
}

void display()
{
	struct node *temp=head;
	while(temp!=NULL)
	{
		printf("%d ",temp->data);
		temp=temp->next;
	}
	printf("\n");
}
