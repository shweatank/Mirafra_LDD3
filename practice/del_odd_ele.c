#include<stdio.h>
void main(){
int arr[6]={1,2,3,4,5,6};
int ele=sizeof(arr)/sizeof(arr[0]);
int i,j=0;
for(i=0;i<ele;i++){
	if(arr[i]%2==0)
	arr[j++]=arr[i];
}
for(i=0;i<j;i++)
printf("%d ",arr[i]);
}

/*delete negative elements
for(i=0;i<ele;i++){
	if(arr[i]>=0)
	arr[j++]=arr[i];
}
*/
