#include<stdio.h>
void print(int row,int col,int arr[row][col])
{
	for(int i=0;i<row;i++)
	{
		for(int j=0;j<col;j++)
			printf("%d ",arr[i][j]);
		printf("\n");
	}
	printf("\n");
}
void input(int r,int c,int arr[r][c])
{
	printf("enter eleemnts:\n");
	for(int i=0;i<r;i++)
		for(int j=0;j<c;j++)
			scanf("%d",&arr[i][j]);
}
void mul(int r1,int c1,int c2,int a[r1][c1],int b[c1][c2],int res[r1][c2])
{
	for(int i=0;i<r1;i++)
		for(int j=0;j<c2;j++)
			res[i][j]=0;

	for(int i=0;i<r1;i++)
		for(int j=0;j<c2;j++)
			for(int k=0;k<c1;k++)
				res[i][j]+=a[i][k]*b[k][j];
}
int main()
{
	int r1,c1,r2,c2;
	printf("enter row and coloum for A:\n");
	scanf("%d%d",&r1,&c1);
	printf("enter row and coloum for B:\n");
	scanf("%d%d",&r2,&c2);
	int a[r1][c1],b[r2][c2],res[r1][c2];
	input(r1,c1,a);
	input(r2,c2,b);
	printf("matrix a:\n");
	print(r1,c1,a);
	printf("matrix b:\n");
	print(r2,c2,b);
	mul(r1,c1,c2,a,b,res);
	printf("matrix res:\n");
	print(r1,c2,res);
}
