#include <stdio.h>
#include <stdlib.h>
typedef struct node
{
    int data;
    struct node *link;
} Node;

Node* create_node(int data)
{
    Node *new_node=calloc(1, sizeof(Node));
    if (!new_node) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    new_node->data=data;
    new_node->link=NULL;
    return new_node;
}

void print_list(Node *head) {
    while(head)
    {
        printf("%d ", head->data);
        head=head->link;
    }
    printf("\n");
}
Node* delete_middle(Node *head) {
    if (!head)
        return NULL;

    if (!head->link) 
    {
        free(head);
        return NULL;
    }

    Node *slow=head,*fast=head,*prev=NULL;
    while (fast && fast->link) 
    {
        prev=slow;
        slow=slow->link;
        fast=fast->link->link;
    }
    prev->link=slow->link;
    free(slow);
    return head;
}

int main() 
{
    Node *head=create_node(10);
    head->link=create_node(20);
    head->link->link=create_node(30);
    head->link->link->link=create_node(40);
    head->link->link->link->link=create_node(50);
    head->link->link->link->link->link=create_node(60);
    printf("Original List: ");
    print_list(head);
    head = delete_middle(head);
    printf("After Deleting Middle Node: ");
    print_list(head);
    return 0;
}
