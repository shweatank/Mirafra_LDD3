#include<stdio.h>
void shift(char *p,int i){
if(p[i]!='\0'){
	p[i]=p[i+1];
	shift(p,i+1);
}
else
return;
}
void delete(char *p,int i){
if(p[i]!=0){
	if(p[i]>='0' && p[i]<='9'){
	shift(p,i);
	delete(p,i);
	}
	else
	delete(p,i+1);
}
else
return;
}
void main(){
char s[20]="h2llo123456";
int i=0;
delete(s,i);
printf("%s\n",s);
}
	
