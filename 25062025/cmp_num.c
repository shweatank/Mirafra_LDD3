/*** check numbers are equal ***/

#include <stdio.h>

int main()
{
   int num1,num2;
   printf("Enter the two number :  ");
   scanf("%d%d",&num1,&num2);

   if(num1 ^ num2)
	   printf("The numbers are not same \r\n");
   else
	   printf("The numbers are same \r\n");
   return 0;
}
