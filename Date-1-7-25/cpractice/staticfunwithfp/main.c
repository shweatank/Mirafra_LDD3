#include<stdio.h>

extern void (*fp())();

int main()
{
	void (*fun_pointer)()=fp();
	fun_pointer();
}
