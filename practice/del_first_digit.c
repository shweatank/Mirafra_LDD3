#include<stdio.h>
void main(){
int arr[5]={123,234,345,456,567};
int i,x;
int ele=sizeof(arr)/sizeof(arr[0]);
for(i=0;i<ele;i++){
	x=1;
	while(arr[i]/x>=10)
	x=x*10;
	arr[i]=arr[i]%x;
	printf("%d ",arr[i]);
}
}

/*
//count set and clear bits in array
#include<stdio.h>
void main(){
int arr[5]={1,234,345,456,567};
int i,j,set,clear;
for(i=0;i<5;i++){
	set=0,clear=0;
	for(j=0;j<31;j++)
	(arr[i]>>j)&1?set++:clear++;
	printf("%d %d\n",set,clear);
}
}

//store set bit count in another array
#include<stdio.h>
void main(){
int arr[5]={1,234,345,456,567};
int i,j,b[5];
for(i=0;i<5;i++){
b[i]=0;
	for(j=0;i<31;j++)
	if(arr[i]>>j&1)
	b[i]++
}
}*/


