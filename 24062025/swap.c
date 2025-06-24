#include <stdio.h>

int main()
{
  int num1,num2;
  printf("Enter two numbers \r\n");
  scanf("%d%d",&num1,&num2);
  printf("Before swapping num1 = %d num2 = %d\r\n",num1,num2);
  num1 = num1 + num2;
  num2 = num1 - num2;
  num1 = num1 - num2;
  printf("After Swaping num1 = %d num2 = %d\r\n",num1,num2);
}
