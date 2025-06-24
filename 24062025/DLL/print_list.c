#include "dll.h"


void print_list(Dlist *head)
{
  if(head == NULL)
  {
    printf("list empty\r\n");
  }
  else
  {
    printf("head ->");
    while(head)
    {
      printf("%d <-",head->data);
      head = head->next;
      if(head)
	      printf("> ");
    }
    printf(" tail \r\n");
  }
}
