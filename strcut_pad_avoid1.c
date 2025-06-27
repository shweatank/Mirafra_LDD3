#include<stdio.h>
struct A{
	int a;
	char b;
	char c;
};
int main(){
	printf("%zu\n",sizeof(struct A));
	return 0;
}

