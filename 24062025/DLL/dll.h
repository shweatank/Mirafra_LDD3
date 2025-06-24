#define DLL_H

#include <stdio.h>
#include <stdlib.h>

#define SUCCESS 0
#define FAILURE -1
#define LIST_EMPTY -2

typedef int data_t;

typedef struct node
{
   data_t data;
   struct node *next;
   struct node *prev;
}Dlist;

int insert_at_first(Dlist **,Dlist **,data_t);
void print_list(Dlist *);
int reverse_list(Dlist **);
