/*#include<stdio.h>
int length(char *);
void main(){
char s[100]="hello";
int len=length(s);
printf("%d",len);
}
int length(char *p){
static int i=0;
int res;
if(p[i]){
	i++;
	length(p);
}
else {
	res=i;
	i=0;
	return res;
	}
}
*/

#include<stdio.h>

int length(char *p);

void main() {
    char s[100] = "hello";
    int len = length(s);
    printf("%d", len);
    len=length(s);
    printf("%d\n",len);
}

int length(char *p) {
    if (*p=='\0')
        return 0;
    else
        return 1 + length(p + 1);
}

