#include<stdio.h>
#include<string.h>
void string_cat(char *s1,char* s){
int len=strlen(s1);
	if(*s=='\0')
		return;
	else{	
		if(*s){
		*(s1+len)=*s;
		return string_cat(s1+1,s+1);
		}
		else
		*s1='\0';
	}
	
}

char *string_chr(char* s,char ch){
	if(*s=='\0')
		return NULL;
		
	if(*s==ch)
		return (char*)s;
	else{
		return string_chr(s+1,ch);
	}
}

	
void main(){
	char s[50]="morning",s1[50]="good",ch='r';
	//string_cat(s1,s);
	//printf("%s\n",s1);
	char *chh=string_chr(s,ch);
	printf("%s\n",chh);
	
}
