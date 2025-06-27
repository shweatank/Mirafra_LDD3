#include<stdio.h>
#include<stdlib.h>

struct node{
	int data;
	struct node *next;
};

void insertend(struct node **head,int value){
	struct node *newnode=malloc(sizeof(struct node));
	newnode->data=value;
	newnode->next=NULL;

	if(*head==NULL){
		*head=newnode;
		return ;
	}

	struct node *temp=*head;
	while(temp->next)
		temp=temp->next;

	temp->next=newnode;
}

void display(struct node *head){
	while(head){
		printf("%d->",head->data);
		head=head->next;
	}
	printf("NULL\n");
}

void freelist(struct node *head){
	struct node *temp;
	while(head){
		temp=head;
		head=head->next;
		free(temp);
	}
}

int main(){
	struct node *head=NULL;

	insertend(&head,10);
	insertend(&head,20);
	insertend(&head,30);

	display(head);
	freelist(head);
	return 0;
}


