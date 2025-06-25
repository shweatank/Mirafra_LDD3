/*** swap ith and jth bit ***/

#include <stdio.h>


unsigned int swap(int n,int i,int j)
{
   int bit_i = (n >> i) & 1;
   int bit_j = (n >> j) & 1;

   if(bit_i != bit_j)
   {
     n ^= (1U << i);
     n ^= (1U << j);
   }
   return n;
}
int main()
{
   unsigned int data;	
   printf("Enter the number : ");
   scanf("%u",&data);
   int i,j;
   printf("Enter the ith and jth position : ");
   scanf("%d%d",&i,&j);
   unsigned int result = swap(data,i,j);
   printf("Swaped value is : %u \r\n",result);
   return 0;
}
