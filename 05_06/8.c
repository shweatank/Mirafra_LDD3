#include<stdio.h>
int length(char *);
int prime(int ,int);
void main(){
char s[20]="pawan";

int len=length(s);
int res=prime(len ,0);
if(res==2)
printf("prime\n");
else
printf("not\n");
}
int length(char *p){
if(*p)
	return 1+length(p+1);
else
	return 0;
}
int prime(int len,int c){
static int k=1;
int res;
if(k<=len){
	if(len%k==0)
	c++;
	k++;
	return prime(len,c);
}
else{
res=c;
k=1;
return res;
}
}
	
	
	
