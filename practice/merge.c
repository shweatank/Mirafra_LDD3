#include<stdio.h>
void main(){
int arr1[3]={1,3,5};
int arr2[5]={2,4,6,8,0};
int c[10];
int ele1=sizeof(arr1)/sizeof(arr1[0]);
int ele2=sizeof(arr2)/sizeof(arr2[0]);
int i=0,j=0,k=0;
while(i<ele1||j<ele2){
	if(i<ele1)
	c[k++]=arr1[i++];
	if(j<ele2)
	c[k++]=arr2[j++];
}
for(i=0;i<k;i++)
printf("%d ",c[i]);
}
