#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Node 
{
    char data;
    struct Node* next;
};

int main() 
{
    struct Node* head = NULL, *temp = NULL, *tail = NULL;
    int i, n, k;

    printf("Enter number of nodes: ");
    scanf("%d", &n);

    printf("Enter characters:\n");
    for (i = 0; i < n; i++) {
        char ch;
        scanf(" %c", &ch);

        temp = (struct Node*)malloc(sizeof(struct Node));
        temp->data = ch;
        temp->next = NULL;

        if (head == NULL) {
            head = temp;
            tail = temp;
        } else {
            tail->next = temp;
            tail = temp;
        }
    }

    printf("Enter k: ");
    scanf("%d", &k);

    if (k > n || k <= 0) {
        printf("Invalid k\n");
        return 0;
    }

    if (2 * k - 1 == n) {
        printf("Swapping same node, not possible.\n");
    } else {
        struct Node *prevX = NULL, *currX = head;
        for (i = 1; i < k; i++) {
            prevX = currX;
            currX = currX->next;
        }

        struct Node *prevY = NULL, *currY = head;
        for (i = 1; i < n - k + 1; i++) {
            prevY = currY;
            currY = currY->next;
        }

        if (prevX)
            prevX->next = currY;
        else
            head = currY;

        if (prevY)
            prevY->next = currX;
        else
            head = currX;

        struct Node* tempPtr = currX->next;
        currX->next = currY->next;
        currY->next = tempPtr;
    }

    printf("Modified list after swapping:\n");
    temp = head;
    while (temp != NULL) {
        printf("%c ", temp->data);
        temp = temp->next;
    }
    printf("\n");

    return 0;
}

