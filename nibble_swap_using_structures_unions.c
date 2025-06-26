#include<stdio.h>
union swap
{
	int num;
	
	struct st
	{
		unsigned char low:4;
		unsigned char high:4;
	}s;
};

int main()
{
	union swap u;
	u.num=23;
	//printf("%d %d\n",u.s.low,u.s.high);
	unsigned char temp=u.s.low;
	u.s.low=u.s.high;
	u.s.high=temp;
	printf("%d",u.num);
	
}

