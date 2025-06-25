/*** power of two or not ***/

#include <stdio.h>

int p_two(int n)
{
  return (n > 0 && ((n & (n - 1)) == 0)) ? 1 : 0;
}
int main()
{
  int n;
  printf("Enter the value :");
  scanf("%d",&n);
  if(p_two(n))
	  printf("%d is power of two \r\n",n);
  else
	  printf("%d is not power of two \r\n",n);
  return 0;
}
