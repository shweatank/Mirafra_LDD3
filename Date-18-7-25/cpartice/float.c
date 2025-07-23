#include<stdio.h>
union u
{
	float f;
	int x;
};
int main()
{
	union u v={25.5};

	int i;
	for(i=31;i>=0;i--)
		printf("%d",v.x>>i&1);
	printf("\n");
        
	int count=0;
	while(v.x)
	{
		count+=v.x&1;
		v.x>>=1;
	}
	printf("%d\n",count);
		
	

}

