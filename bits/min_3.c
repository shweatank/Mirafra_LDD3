#include<stdio.h>
#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) \
                                  : ((b) < (c) ? (b) : (c)))
int main()
{
	printf("%d",MIN3(1,2,5));
}

