#include <stdio.h>

int my_atoi(const char *str)
{
   int i = 0,sign = 1,result = 0;

   while(str[i] == ' '|| str[i] == '\t') i++;

   if(str[i]=='-' || str[i] == '+')
   {
     if(str[i] == '-')
	     sign = -1;
     i++;
   }
   while(str[i] >= '0' && str[i] <= '9')
   {
      result = result * 10 + (str[i] - '0');
      i++;
   }
   return sign * result;
}
int main()
{
  char str[100];
  scanf("%[^\n]s",str);

  printf("The result : %d\r\n",my_atoi(str));
  return 0;
}
