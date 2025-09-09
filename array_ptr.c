#include<stdio.h>
int  main()
{
  int arr[]={1,2,3,4};
  int (*ptr)[4];
  ptr=arr;
  for(int i=0;i<4;i++)
  {
     printf("%u\n",&(*ptr)[i]);
  }
}


