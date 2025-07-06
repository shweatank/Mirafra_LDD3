#include<stdio.h>
#include<string.h>
void shift(char *p,int i){
if(p[i]!='\0'){
p[i]=p[i+1];
shift(p,i+1);
}
else
return ;
}
void delete(char *p,int len,char ch,int n){
if(len>=0){
	if(p[len]==ch && n>0){
	shift(p,len);
	n--;
	delete(p,len,ch,n);
	}
	else
	delete(p,len-1,ch,n);
}

}
void main(){
char s[20]="hello lilly";
char ch='l';
int len=strlen(s)-1;
delete(s,len,ch,2);
printf("%s",s);
}




