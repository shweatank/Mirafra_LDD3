#include<stdio.h>
int main()
{
    int n;
    scanf("%d",&n);
    int cnt=0;
    for(int bit=31;bit>=1;bit--)
    {
        if((n>>bit)&1)
        {
            if((n>>(bit-1))&1)
            {
                cnt++;
                bit--;
            
            }
        }
        
        
    }
    printf("%d\n",cnt);
}
