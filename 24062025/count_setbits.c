#include <stdio.h>

int count_bits(int n)
{
  int count = 0;
  while(n)
  {
   if(n & 1) count++;
   n >>= 1;
  }
  return count;
}
int main()
{
   int num;
   printf("Enter the number \r\n");
   scanf("%d",&num);
   printf("No. of Set bits : %d\r\n",count_bits(num));
   return 0;
}
