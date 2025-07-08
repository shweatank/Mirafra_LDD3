#include<stdio.h>
#include<string.h>
int length(char *s){
	int len=0;
	while(s[len]!='\0')
	len++;
	return len;
}
int length_rec(char *s){
	int len;
	if(*s=='\0')
		return 0;
	return 1+length_rec(s+1);
}

void string_copy(char *d,char *s){
	int i;
	while(*s!='\0'){
		*d=*s;
		s++;d++;
	}
	*d='\0';
}
void string_copy_rec(char *d ,char *s){
	if(*s=='\0')
		return;
	else{
		*d=*s;
		return string_copy_rec(d+1,s+1);
	}
}
	
int string_cmp(char *s,char *s1){
	while(*s &&*s1){
		if(*s!=*s1)
		return 0;
	s++;s1++;
	}
	return(*s==*s1);
	//return 1;
}

int string_cmp_rec(char *s,char *s1){
	if(*s!=*s1)
	return 0;
	if(*s=='\0')
	return 1;
	return string_cmp_rec(s+1,s1+1);
}
void string_rev(char *s){
	int len=strlen(s);
	char *p=s+len-1,t;
	while(s<p){
		t=*s;
		*s=*p;
		*p=t;
		s++;p--;
	}
}
					
void main(){
	char s[50],d[50];
	char s1[50]="hi how r u";
	printf("enter the string\n");
	scanf("%[^\n]",s);
	//int result=length(s);
	//int result=length_rec(s);
	//printf("length is %d\n",result);
	
	//string_copy_rec(d,s);
	//printf("string is %s\n",d);
	
	/*int result=string_cmp_rec(s,s1);
	if(result==0)
	printf("not\n");
	else
	printf("equal\n");
	*/
	string_rev(s);
	printf("rev is %s\n",s);
}
