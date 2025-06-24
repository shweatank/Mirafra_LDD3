#include "sll.h"


void reverse_list(Slist **head)
{
    if(*head == NULL)
	    printf("List is empty\r\n");
    else
    {
	Slist *prev = NULL;
	Slist *next = NULL;
	Slist *curr = *head;

	while(curr != NULL)
	{
	    next = curr->link;
	    curr->link = prev;
	    prev = curr;
	    curr = next;
	}
	*head = prev;

    }
}
