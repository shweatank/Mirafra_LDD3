static int add(int a,int b)
{
	return a+b;
}

int (*fp)(int,int)=add;
