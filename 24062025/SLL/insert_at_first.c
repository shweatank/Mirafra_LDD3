#include "sll.h"


int insert_at_first(Slist **head,data_t data)
{
   Slist *new = malloc(sizeof(Slist));
   if(new == NULL)
   {
	printf("Memory allocation failed \r\n");
	return FAILURE;
   }
   new->data = data;
   new->link = NULL;
   if(*head == NULL)
   {
      *head = new;
 
   }
   else
   {
      Slist *temp = *head;
      *head = new;
      new->link = temp;
   }
   return SUCCESS;	
}
