#include "sll.h"

void print_list(Slist *head)
{
   if(head == NULL)
   {
     printf("The list is empty\r\n");
   }
   else
   {
     while(head)
     {
	printf("%d -> ",head->data);
	head = head->link;
     }
     printf("NULL\r\n");
   }
}
