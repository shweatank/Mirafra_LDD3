#include<stdio.h>
void main(){
int arr[6]={1,2,3,-1,-2,-3};
int ele=sizeof(arr)/sizeof(arr[0]);
int i,j,k=0,x;
for(i=0;i<ele;i++){
	x=arr[k];
	if(x<0){
		for(j=k;j<ele;j++)
		arr[j]=arr[j+1];
		arr[ele-1]=x;
	}
	else
		k++;
}
for(i=0;i<ele;i++)
printf("%d ",arr[i]);
printf("\n");
}
	
