//insert element inn particular index
/*
#include<stdio.h>
void main(){
int arr[6]={1,2,3,4,5},num=10,ind=3,i;
int ele=sizeof(arr)/sizeof(arr[0]);
	for(i=ele-1;i>ind;i--)
		arr[i]=arr[i-1];
		arr[ind]=num;
	for(i=0;i<ele;i++)
	printf("%d ",arr[i]);
}
*/
#include<stdio.h>
void main(){
	int arr[7]={1,2,3,7,11,13},i;
	int ele=sizeof(arr)/sizeof(arr[0]);
	for(i=0;i>=0 && arr[i]>num;i--)
		arr[i+1]=arr[i];
	arr[i+1]=num;
	for(i=0;i<7;i++)
	printf("%d ",arr[i]);
}
		
	
