//sum of first digit of all elements
#include<stdio.h>
void main(){
int arr[5]={123,234,345,456,567};
int i,sum=0,temp;
int ele=sizeof(arr)/sizeof(arr[0]);
for(i=0;i<ele;i++){
	temp=arr[i];
	while(temp>=10)
	temp=temp/10;
	sum=sum+temp;
}
printf("sum is %d\n",sum);
}
