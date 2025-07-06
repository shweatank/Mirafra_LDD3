#include<stdio.h>
#include<stdlib.h>
void main()
{
	int **p,**q,**re;
	int r,c,i,j;
	printf("Enter the number of rows adn columns : ");
	scanf("%d%d",&r,&c);
	p=(int **)malloc(sizeof(int *));
	for(i=0;i<r;i++)
		p[i]=(int *)malloc(sizeof(int));
	q=(int **)malloc(sizeof(int *));
	for(i=0;i<r;i++)
		q[i]=(int *)malloc(sizeof(int));
	re=(int **)malloc(sizeof(int *));
	for(i=0;i<r;i++)
		re[i]=(int *)malloc(sizeof(int));
	printf("Enter the values of 1st matrix : ");
	for(i=0;i<r;i++)
	{
		for(j=0;j<c;j++)
			scanf("%d",&p[i][j]);
	}
	printf("Enter the values of 2nd matrix : ");
	for(i=0;i<r;i++)
	{
		for(j=0;j<c;j++)
			scanf("%d",&q[i][j]);
	}
	for(i=0;i<r;i++)
		for(j=0;j<c;j++)
			re[i][j]=p[i][j]+q[i][j];
	for(i=0;i<r;i++)
	{
		for(j=0;j<c;j++)
			printf("%d ",re[i][j]);
		printf("\n");
	}
}
