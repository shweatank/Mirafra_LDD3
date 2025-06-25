/*** returning string from function ***/

#include <stdio.h>

char *get_name()
{
  static char name[100];
  printf("Enter the string : ");
  scanf("%[^\n]s",name);
  return name;

}

int main()
{
  char *result = get_name();
  printf("Returned string is : %s\r\n",result);
  return 0;
}
