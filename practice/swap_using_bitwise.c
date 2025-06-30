//swap first and last elements using bitwise operator
#include<stdio.h>
void main(){
int arr[5]={1,2,3,4,5},i;
int ele=sizeof(arr)/sizeof(arr[0]);
int *p=arr,*q=arr+ele-1;
*p=*p^*q;
*q=*q^*p;
*p=*p^*q;
for(i=0;i<ele;i++)
printf("%d ",arr[i]);
}
