#include<stdio.h>
int main()
{
    

    int data;
    scanf("%d",&data);
    int left,right;
    for(left=31,right=0;left>right;left--,right++)
	{
				if(((data>>left)&1) != ((data>>right)&1))
				{
					data^=(1<<left);
					data^=(1<<right);
				}
	}
  for (int i = 31; i >= 0; i--) {
        printf("%d", (data>> i) & 1);
    }
	printf("  %d\n",data);
	
}
