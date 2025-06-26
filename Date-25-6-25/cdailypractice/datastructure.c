#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct student {
    int rollno;
    char name[20];
    float marks;
    struct student *next;
} ST;

void add_end(ST **head) {
    ST *new = malloc(sizeof(ST));
    printf("Enter rollno name marks:\n");
    scanf("%d %s %f", &new->rollno, new->name, &new->marks);
    new->next = NULL;

    if (*head == NULL) {
        *head = new;
        return;
    }

    ST *temp = *head;
    while (temp->next != NULL)
        temp = temp->next;

    temp->next = new;
}

void print(ST *head) {
    if (head == NULL) {
        printf("List is empty.\n");
        return;
    }
    while (head != NULL) {
        printf("%d %s %.2f\n", head->rollno, head->name, head->marks);
        head = head->next;
    }
}

void reverse_list(ST **head) {
    ST *prev = NULL, *curr = *head, *next = NULL;
    
    while (curr != NULL) {
        next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }
    *head = prev;
}

void reverse_print(ST *head) {
    if (head != NULL) {
        reverse_print(head->next);
        printf("%d %s %.2f\n", head->rollno, head->name, head->marks);
    }
}

int main() {
    ST *head = NULL;
    char ch;

    do {
        add_end(&head);
        printf("Add another (y/n)? ");
        scanf(" %c", &ch);
    } while (ch == 'y' || ch == 'Y');

    printf("\nOriginal list:\n");
    print(head);

    printf("\nReverse print (without changing list):\n");
    reverse_print(head);

    printf("\nReversing the list (links):\n");
    reverse_list(&head);
    print(head);

    return 0;
}

