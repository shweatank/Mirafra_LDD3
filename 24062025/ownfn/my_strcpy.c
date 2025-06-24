#include <stdio.h>

void my_strcpy(char *dest,char *src)
{
   while(*src != '\0')
   {
     *dest = *src;
     src++;
     dest++;
   }
   *dest = '\0';
}
int main()
{
   char str[100],new[100];
   printf("Enter the string : ");
   scanf("%[^\n]s",str);
   my_strcpy(new,str);
   printf("coppied string : %s \r\n",new);

}
