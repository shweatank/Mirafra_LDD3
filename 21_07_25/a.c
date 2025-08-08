//#include<stdio.h>
static int add(int a,int b){
	return a+b;
}

int (*ptr)(int,int)=add;
