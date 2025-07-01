#include<stdio.h>
void main(){
int a[7]={-1,-1,2,2,3,3,4};
int ele=sizeof(a)/sizeof(a[0]);

int diff[1000],count=0,freq[1000]={0},i;
for(int i=0;i<ele;i++){
	freq[a[i]]++;
	if(freq[a[i]]==1)
		diff[count++]=a[i];
}
for(i=0;i<count;i++){
	int val=diff[i];
	if(freq[val]>1)
	printf("%d %d\n",val,freq[val]);
}
} 
