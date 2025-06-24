#include "dll.h"

int insert_at_first(Dlist **head,Dlist **tail,data_t data)
{
   Dlist *new = malloc(sizeof(Dlist));
   if(new == NULL)
   {
     printf("Memory allocation failed\r\n");
     return FAILURE;
   }
   new->data = data;
   new->next = NULL;
   new->prev = NULL;

   if(*head == NULL)
   {
     *head = *tail = new;
     return SUCCESS;
   }
     Dlist *temp = *head;
     new->next = temp;
     temp->prev = new;
     *head = new;
    return SUCCESS; 
}
