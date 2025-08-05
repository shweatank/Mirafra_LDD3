#include<stdio.h>
#include<string.h>

void deldup(int *a, int *count) {
    int c = *count;

    for (int i = 0; i < c; i++) {
        for (int j = i + 1; j < c; ) {
            if (a[i] == a[j]) {
                // Shift left to remove duplicate
                memmove(&a[j], &a[j + 1], (c - j - 1) * sizeof(int));
                c--;
            } else {
                j++;
            }
        }
    }

    *count = c;
}

int main()
{
	int a[10],count=0;
	for(int i=0;i<10;i++)
	{
		scanf("%d",&a[i]);
		count++;
	}
	for(int i=0;i<count;i++)
	{
		printf("%d ",a[i]);
	}
	printf("\n");
	deldup(a,&count);
	for(int i=0;i<count;i++)
	{
		printf("%d ",a[i]);
	}
	printf("\n");
	return 0;
}
