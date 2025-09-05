#include<stdio.h>
void display(void)
{
	printf("iam in display function\n");
}
int main()
{
	void (*ptr)(void);
	ptr=display;
	ptr();
}
