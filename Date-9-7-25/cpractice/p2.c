#include <stdio.h>

#define MAX 10000  

int main() 
{
    int n, i;
    printf("Enter the number of elements:\n");
    scanf("%d", &n);

    int a[n];               
    int count[MAX] = {0};    

    printf("Enter the elements:\n");
    for (i = 0; i < n; i++) 
    {
        scanf("%d", &a[i]);     
        count[a[i]]++;          
    }
    printf("Element --> Frequency\n");
    for (i = 0; i < n; i++) 
    {
        if (count[a[i]] != 0) 
	{              
            printf("%d ---> %d\n", a[i], count[a[i]]);
            count[a[i]] = 0;                  
        }
    }

    return 0;
}

