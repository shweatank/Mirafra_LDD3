#include <stdio.h>
int main()
{
    int n ;
    scanf("%d",&n);
    int max_gap = 0;
    int last_index = -1;
    int index = 0;

    while (n > 0)
    {
        if (n & 1)
       	{
            if (last_index != -1) 
	    {
                int gap = index - last_index;
                if (gap > max_gap)
                    max_gap = gap;
            }
            last_index = index;
        }
        n >>= 1;
        index++;
    }
    printf("%d\n", max_gap);
    return 0;
}

