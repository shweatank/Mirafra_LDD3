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
void delete(char *p){
shift(p,0);

}
void main(){
char s[20]="hello lilly";
char ch='l';
int len=strlen(s);
int i=0;
delete(s);
printf("%s",s);
}




