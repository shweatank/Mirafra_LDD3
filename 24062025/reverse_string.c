#include <stdio.h>
#include <string.h>

void reverse_str(char *str)
{
   int start = 0, end = strlen(str) - 1;

   while(start < end)
   {
     char temp = str[start];
     str[start] = str[end];
     str[end] = temp;
     start++;
     end--;
   }
   
}
int main()
{
   char str[100];
   printf("Enter the string :");
   scanf("%[^\n]s",str);
   reverse_str(str);
   printf("Reverse string : %s\r\n",str);
}
