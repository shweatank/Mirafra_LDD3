#include<stdio.h>
void set_bit(char *);
int count (char);
void main(){
char s[100]="bcd12";
set_bit(s);
}
void set_bit(char *p){
int c;
if(*p){
	c=count(*p);
	printf("count is %d\n",c);
	return set_bit(p+1);
}
}
int count(char ch){
	static int pos=31,c=0;
	int res;
	if(ch>='0' && ch<='9')
	ch=ch-'0';
	if(pos>=0){
		if(ch>>pos&1)
		c++;
		pos--;
		return count(ch);
	}
	else
	{	
		res=c;
		pos=31,c=0;
		return res;
	}
}
	









