#include<stdio.h>
#include"1.h"
void main(){
	int a,b;
	printf("Enter numbers to add:");
	scanf("%d%d",&a,&b);
	int result=ptr(a,b);
	printf("%d\n",result);
}
