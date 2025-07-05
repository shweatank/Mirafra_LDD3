#include<stdio.h>
void print_binary(int n)
{
        int bit=31;
        while(bit>=0)
        {
                printf("%d",(n>>bit)&1);

                if(bit%8==0 && bit!=0)
                  printf(" ");
                bit--;
        }
        printf("\n");
}
int ones_binary(int n)
{
	 int bit=31;
	 while(bit>=0)
        {
                if((n>>bit)&1)
                {
                        n=n^(1<<bit);
                }
                else
                {
                        n=n^(1<<bit);
                }
                bit--;
        }
	 return n;
}
int twos_binary(int n)
{
	int bit=0;
	while(bit<32)
	{
		if((n>>bit)&1)
			n=n^(1<<bit);
		else
		{
			n=n^(1<<bit);
			break;
		}
		bit++;
	}
	return n;
}		 
int main()
{
	int x,y, num;
	printf("enter the number\n");
	scanf("%d",&num);
	printf("the number before 2's compliment:%d\n",num);
	print_binary(num);
	x=ones_binary(num);
	printf("the number after 1's compliment:%d\n",x);
        print_binary(x);
	y=twos_binary(x);
	printf("the number 2's compliment:%d\n",y);
        print_binary(y);
}




