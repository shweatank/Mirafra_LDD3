#include <stdio.h>

int my_strcmp(const char*str1,const char *str2)
{
   while(*str1 != '\0' || *str2 != '\0')
   {
     if(*str1 != *str2)
     {
	return -1;
     }
     str1++;
     str2++;
   }
   return 0;
}
int main()
{
   char str1[100],str2[100];

   printf("Enter the first string : \r\n");
   scanf(" %[^\n]s",str1);
   printf("Enter the second string : \r\n");
   scanf(" %[^\n]s",str2);
  if(!my_strcmp(str1,str2))
	  printf("Strings are equal\r\n");
  else
	  printf("strings are not equal\r\n");
   return 0;
}
