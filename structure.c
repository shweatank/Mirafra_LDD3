#include<stdio.h>
int addition(int a,int b){
	return a+b;
}
int subtraction(int a,int b){
	return a-b;
}
int multiplication(int a,int b){
	return a*b;
}
int division(int a,int b){
	return a/b;
}



struct operation{
	int(*add)(int,int);
	int(*sub)(int,int);
	int(*mul)(int,int);
	int(*div)(int,int);
};
int main(){

	struct operation op={.add=addition,
			     .sub=subtraction,
			     .mul=multiplication,
			     .div=division};
	//op.add=addition;
	//op.sub=subtraction;
	
	int a=3,b=2;
	printf("%d\n",op.add(a,b));
	printf("%d\n",op.sub(a,b));
	printf("%d\n",op.mul(a,b));
}
	

