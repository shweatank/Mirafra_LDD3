#include<stdio.h>
int fact(int n){
	int f=1;
	while(n>1)
		f=f*n--;
	return f;
}
int prime(int n){
	if(n<2)
		return 0;
		
	for(int i=2;i*i<=n;i++)
		if(n%i==0)
			return 0;
		return 1;
}
void main(){
int a[5]={3,4,5,6,7},i,j,c=0;
int ele=sizeof a/sizeof *a;
int b[ele];
for(i=0;i<ele;i++){
	b[i]=fact(a[i]);
	printf("%d ",b[i]);
	
}
printf("\n");
for(i=0;i<ele;i++){
	if(prime(a[i])){
	printf("%d ",a[i]);
	c++;
	}
}

printf("\ncount is %d \n",c);
}
