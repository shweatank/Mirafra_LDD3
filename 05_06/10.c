#include<stdio.h>
#include<string.h>
void reverse(char *);
void main(){
char s[20]="hello world";
reverse(s);
printf("%s",s);
}
void reverse(char* p){
static int i=0;
int t;
int j=strlen(p)-1-i;
if(i<j){
	t=p[i];
	p[i]=p[j];
	p[j]=t;
	i++;
	reverse(p);
}
else
i=0;
}

#include <stdio.h>
#include <string.h>

void reverse(char*, int, int);

void main() {
    char s[20] = "hello world";
    reverse(s, 0, strlen(s) - 1);
    printf("%s", s);
}

void reverse(char* p, int start, int end) {
    if (start >= end) {
        return;
    }
    char temp = p[start];
    p[start] = p[end];
    p[end] = temp;
    reverse(p, start + 1, end - 1);
}
	

