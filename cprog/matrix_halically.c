#include<stdio.h>
/* HELICAL MATRIX */
int main()
{
int arr[][4] = { {1,2,3,4},
{5,6,7,8},
{9,10,11,12},
{13, 14, 15, 16}
};
int i, j, k,middle,size;
printf("\n\n");
size = 4;
for(i=size-1, j=0; i>0; i--, j++)
{
for(k=j; k<i; k++) printf("%d ", arr[j][k]);
printf("\n");
for(k=j; k<i; k++) printf("%d ", arr[k][i]);
printf("\n");
for(k=i; k>j; k--) printf("%d ", arr[i][k]);
printf("\n");
for(k=i; k>j; k--) printf("%d ", arr[k][j]);
printf("\n");
}
middle = (size-1)/2;
if (size % 2 == 1) printf("%d", arr[middle][middle]);
printf("\n\n");
return 1;
}
