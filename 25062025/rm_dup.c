/*** delete duplicate from array ***/

#include <stdio.h>
void remove_dup(int *arr,int size)
{
   int dup = 0;
   for(int i = 0; i < size; i++)
   {
     int found = 0;
     for(int j = 0; j < i; j++)
     {
	if(arr[i] == arr[j])
	{
	   found = 1;
	   break;
	}
			
     }
     if(!found)
	     printf("%d ",arr[i]);
   }
   printf("\n");
}
int main()
{
   int size;
   printf("Enter the sizeof the the array : ");
   scanf("%d",&size);
   int arr[size];
   printf("Enter the elements : ");
   for(int i = 0; i < size; i++)
   {
      scanf("%d",&arr[i]);
   }
   printf("Array after removing duplicates : ");
   remove_dup(arr,size);
   return 0;
}
