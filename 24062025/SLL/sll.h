#define SLL_H

#include <stdio.h>
#include <stdlib.h>

#define SUCCESS 0
#define FAILURE -1
#define LIST_EMPTY -2
typedef int data_t;

typedef struct node
{
  data_t data;
  struct node *link;
}Slist;

int insert_at_first(Slist **,data_t);
void reverse_list(Slist **);
void print_list(Slist *);
