/*** string paliandrom or not ***/

#include <stdio.h>
#include <string.h>

int paliandrom(const char *str)
{
   int start = 0, end = strlen(str) - 1;
   while(start < end)
   {
     if(str[start] != str[end])
	     return 0;
     start++;
     end--;
   }
   return 1;
}
int main()
{
   char str[100];
   printf("Enter the string : ");
   scanf("%[^\n]s",str);
   if(paliandrom(str))
	   printf("Given string is paliandrom \r\n");
   else
	   printf("The given string is not in paliandrom \r\n");
   return 0;
}
