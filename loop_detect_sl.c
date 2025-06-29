#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node *next;
}Node;

// Function to create a new node
Node * newNode(int data) {
    Node* temp = (struct Node*)malloc(sizeof(struct Node));
    temp->data = data;
    temp->next = NULL;
    return temp;
}

// Function to detect loop in the list
int detectLoop(Node* head) {
    Node *slow = head, *fast = head;

    while (slow && fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;

        if (slow == fast) {
            return 1;  // Loop detected
        }
    }

    return 0;  // No loop
}

// Main function to test above
int main() {
    Node* head = newNode(1);
    head->next = newNode(2);
    head->next->next = newNode(3);
    head->next->next->next = newNode(4);
    head->next->next->next->next = newNode(5);

    // Create a loop for testing
    head->next->next->next->next->next = head->next->next; // Loop at node 3

    if (detectLoop(head))
        printf("Loop found in linked list.\n");
    else
        printf("No loop in linked list.\n");

    return 0;
}

