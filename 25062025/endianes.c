/*** check the endianes of the system ***/

#include <stdio.h>

int main()
{
   unsigned int val = 1;
   char *data = (char *)&val;

   if(*data == 1)
	   printf("Little endian system\r\n");
   else
	   printf("Big endian system\r\n");
   return 0;
}
