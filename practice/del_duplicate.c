/*#include<stdio.h>
void main(){

int isPresent(int *res,int size,int val){
	int i;
	for(i=0;i<size;i++){
	if(res[i]==val)
		return 1;
	
	}
	return 0;
}
int remove_duplicates(int *arr,int n,int *res){
int i,k=0;
for(i=0;i<n;i++){
	if(!isPresent(res,k,arr[i]))
	res[k++]=arr[i];
}
return k;
}
	
int arr[6]={1,2,3,1,2,3};
int res[6],i;
int new_size=remove_duplicates(arr,6,res);
for(i=0;i<new_size;i++)
printf("%d ",res[i]);
}
*/

#include<stdio.h>
#define offset 100
#define size 201
int removee(int *arr,int n){
	char seen[size]={0};
	int j=0,i;
	for(i=0;i<n;i++){
		int index=arr[i]+offset;
		if(!seen[index]){
			seen[index]=1;
			arr[j++]=arr[i];
		}
	}
	return j;
}
int main(){
int arr[7]={-1,-2,-1,-2,3,4,4},i;
int ele=sizeof(arr)/sizeof(arr[0]);
ele=removee(arr,ele);
for(i=0;i<ele;i++)
printf("%d ",arr[i]);
}
