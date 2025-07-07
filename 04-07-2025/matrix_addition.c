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
void add(int row,int col,int a[row][col],int b[row][col],int res[row][col])
{
	for(int i=0;i<row;i++)
		for(int j=0;j<col;j++)
			res[i][j]=a[i][j]+b[i][j];
}
int main()
{
	int rows,cols;
	printf("enter rows and cols:\n");
	scanf("%d%d",&rows,&cols);
	int a[rows][cols],b[rows][cols],res[rows][cols];
	printf("enter eleemnts for matrix a:\n");
	for(int i=0;i<rows;i++)
		for(int j=0;j<cols;j++)
			scanf("%d",&a[i][j]);
	printf("enter eleemnts for matrix b:\n");
	for(int i=0;i<rows;i++)
		for(int j=0;j<cols;j++)
			scanf("%d",&b[i][j]);
	printf("matrix a:\n");
	print(rows,cols,a);
	printf("matrix b:\n");
	print(rows,cols,b);
	add(rows,cols,a,b,res);
	printf("matrix res:\n");
	print(rows,cols,res);
}
