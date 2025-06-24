#include "dll.h"

int main()
{
   Dlist *head = NULL;
   Dlist *tail = NULL;
   int data,opt;
   while(1)
   {
     printf("1.insert first\r\n2.reverse list\r\n3.print list\r\n4.exit\r\nEnter the option ");
     scanf("%d",&opt);
     switch(opt)
     {
	case 1:
		printf("Enter the data ");
		scanf("%d",&data);
		if(insert_at_first(&head,&tail,data)==FAILURE)
			printf("insertion failed\r\n");
		break;
	case 2:
		reverse_list(&head);
		break;
	case 3:
		print_list(head);
		break;
	case 4:
		return SUCCESS;
	default:
		printf("Enter valid options\r\n");
		break;
     }
   }
   return 0;
}
