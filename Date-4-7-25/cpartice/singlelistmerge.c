#include <stdio.h>
#include <stdlib.h>

struct Node {
    int data;
    struct Node* next;
};

int main() {
    struct Node *head1 = NULL, *head2 = NULL, *merged = NULL;
    struct Node *temp, *tail;
    int n1, n2, val, i;

    printf("Enter number of elements in first list: ");
    scanf("%d", &n1);

    for (i = 0; i < n1; i++) {
        printf("Enter value %d: ", i + 1);
        scanf("%d", &val);

        temp = (struct Node*)malloc(sizeof(struct Node));
        temp->data = val;
        temp->next = NULL;

        if (head1 == NULL) {
            head1 = temp;
            tail = temp;
        } else {
            tail->next = temp;
            tail = temp;
        }
    }

    printf("\nEnter number of elements in second list: ");
    scanf("%d", &n2);

    for (i = 0; i < n2; i++) {
        printf("Enter value %d: ", i + 1);
        scanf("%d", &val);

        temp = (struct Node*)malloc(sizeof(struct Node));
        temp->data = val;
        temp->next = NULL;

        if (head2 == NULL) {
            head2 = temp;
            tail = temp;
        } else {
            tail->next = temp;
            tail = temp;
        }
    }

    if (head1 == NULL)
        merged = head2;
    else if (head2 == NULL)
        merged = head1;
    else {
        merged = head1;
        temp = head1;
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = head2;
    }

    printf("\nMerged Linked List:\n");
    temp = merged;
    while (temp != NULL) {
        printf("%d -> ", temp->data);
        temp = temp->next;
    }
    printf("\n");

    return 0;
}

