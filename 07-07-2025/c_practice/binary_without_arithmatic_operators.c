#include<stdio.h>
void print_binary(int num)
{
	int flag=0;
	for(int i=31;i>=0;i--)
	{
		if(num&(1u<<i))
		{
			putchar('1');
			flag=1;
		}
		else if(flag)
			putchar('0');
	}
	if(!flag)
		putchar('0');
	printf("\n");
}

/*always print all 32 bits including leading zeroes
void print_binary(int num)
{
    for(int i=31;i>=0;i--)
    {
        putchar((num&(1u<<i))?'1':'0');
    }
    printf("\n");
}*/

int main()
{
	int num;
	printf("enter a num:\n");
	scanf("%d",&num);
	print_binary(num);
}
