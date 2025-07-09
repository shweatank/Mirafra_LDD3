#include<stdio.h>
int add(int a,int b)
{
	return(a+b);
}
int main()
{
	printf("sum:%d\n",add(3,4));
}
/*gcc -c mathlib.c -o mathlib.o         # Compile to object file
ar rcs libmath.a mathlib.o            # Create static library
gcc mathlib.o libmath.a -o static_app # Link statically
./static_app                          # Run*/

