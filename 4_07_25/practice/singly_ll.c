#include<stdio.h>
#include<stdlib.h>
struct node
{
	int data;
	struct node *next;
};

struct node *head=NULL;
void create()
{
	struct node *newnode=NULL,*temp=NULL;
	int choice=1,info=0;
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
		newnode->next=NULL;
		if(head==NULL)
		{
			head=temp=newnode;
		}
		else
		{
			temp->next=newnode;
			temp=newnode;
		}
		printf("Do you want to continue ");
		scanf("%d",&choice);
	}
}

void display()
{
	struct node *temp=head;
	if(temp==NULL)
	{
		printf("List is empty\n");
		return;
	}
	while(temp!=NULL)
	{
		printf("%d ",temp->data);
		temp=temp->next;
	}
	printf("\n");
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
	if(head==NULL)
	{
		head=newnode;
		return;
	}
	newnode->next=head;
	head=newnode;
}

void ins_at_end(int info)
{
	struct node *newnode=NULL,*temp=head;
	newnode=(struct node *)malloc(sizeof(struct node));
	if(newnode==NULL)
	{
		printf("Malloc error\n");
		return;
	}
	newnode->data=info;
	newnode->next=NULL;
	if(head==NULL)
	{
		head=newnode;
		return;
	}
	while(temp->next!=NULL)
	{
		temp=temp->next;
	}
	temp->next=newnode;
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
		printf("Invalid Position\n");
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
	if(temp==NULL)
	{
		printf("List is empty\n");
		return;
	}
	head=head->next;
	free(temp);
}

void del_from_end()
{
	struct node *temp=head,*prev=NULL;
	if(temp==NULL)
	{
		printf("List is empty\n");
		return;
	}
	if(temp->next==NULL)
	{
		head=temp->next;
		free(temp);
		return;
	}
	while(temp->next!=NULL)
	{
		prev=temp;
		temp=temp->next;
	}
	prev->next=NULL;
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

void reverse()
{
	struct node *prev=NULL,*current=head,*nextnode=head;
	while(nextnode!=NULL)
	{
		nextnode=current->next;
		current->next=prev;
		prev=current;
		current=nextnode;
	}
	head=prev;
}

void middlenode()
{
	struct node *slow=head,*fast=head;
	while(fast!=NULL&&fast->next!=NULL)
	{
		slow=slow->next;
		fast=fast->next->next;
	}
	printf("%d\n",slow->data);
}

int main()
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
		printf("9.Middlenode\n");
		printf("10.Display\n");
		printf("11.Exit\n");
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
			case 9: middlenode();
				break;
			case 10: display();
				break;
			case 11: printf("Exit\n");
				exit(0);
			default: printf("Invalid input\n");
		}
	}	
	return 0;
}

