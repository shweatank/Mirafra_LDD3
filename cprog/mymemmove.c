#include<stdio.h>

char *mymemmove(char *dst,char *src,ssize_t size)
{
	if(src==dst || size==0)
	{
		return dst;	
	}
	else if(src>dst)
	{
		for(int i=0;i<size;i++)
		{
			dst[i]=src[i];
		}
	}
	else
	{
		for(int i=size-1;i>=0;i--)
		{
                     dst[i]=src[i];
		}
	}
	return dst;
}
int main()
{
	char str[]="pavan satya swaroop\0";
	mymemmove(str+6,str+12,8);
	puts(str);
}
