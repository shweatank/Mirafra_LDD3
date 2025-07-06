/*#include<stdio.h>
int count(char *);
void main(){
char s[100]="hello 123";
int res=count(s);
printf("count is %d\n",res);
}
int count(char *p){
static int i=0,c=0;
int res;
if(p[i]){
	if(p[i]>='0' && p[i]<='9')
	c++;
	i++;
	return count(p);
}
else{
	res=c;
	c=0,i=0;
	return res;
}
}
*/
#include<stdio.h>
int count(char *);
void main(){
char s[100]="hello 123";
int res=count(s);
printf("count is %d\n",res);
}
int count(char *p){
	if(*p){
		if(*p>='0' && *p<='9')
		return 1+count(p+1);
		else
			return count(p+1);
	}
	else
		return 0;
}

#include<stdio.h>

int count(char *, int);

void main() {
    char s[100] = "hello 123";
    int res = count(s, 0);
    printf("count is %d\n", res);
}

int count(char *p, int index) {
    if (p[index] == '\0')
        return 0;

    if (p[index] >= '0' && p[index] <= '9')
        return 1 + count(p, index + 1);
    else
        return count(p, index + 1);
}











	
	
	
