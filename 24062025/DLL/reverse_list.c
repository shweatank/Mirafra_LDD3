#include "dll.h"

int reverse_list(Dlist **head)
{
   
   if(*head == NULL)
	   return FAILURE;
   Dlist *temp = NULL;
   Dlist *curr = *head;
 
   
   while(curr)
   {
     temp = curr->prev;
     curr->prev = curr->next;
     curr->next = temp;

     curr = curr->prev;
   }
   *head = temp->prev;
   return SUCCESS;
}
