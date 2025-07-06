#include<stdio.h>
#include<stdlib.h>
void main()
{
	int r1,r2,c1,c2,i,j,k;
	int **p,**q,**r;
	printf("Enter the dimensions of 1st matrix :");
	scanf("%d%d",&r1,&c1);
	printf("Enter the dimensions of 2nd matrix :");
	scanf("%d%d",&r2,&c2);
	if(c1!=r2)
	{
		printf("This dimension matrices cannot be multiplied...\n");
		return;
	}
	p=(int **)malloc(sizeof(int *)*r1);
	for(i=0;i<r1;i++)
	{
		p[i]=(int *)malloc(sizeof(int)*c1);
	}
	q=(int **)malloc(sizeof(int *)*r2);
	for(i=0;i<r2;i++)
	{
		q[i]=(int *)malloc(sizeof(int)*c2);
	}
	r=(int **)calloc(r1,sizeof(int *));
	for(i=0;i<r1;i++)
	{
		r[i]=(int *)calloc(c2,sizeof(int));
	}
	printf("Enter the first matrix : ");
	for(i=0;i<r1;i++)
	{
		for(j=0;j<c1;j++)
			scanf("%d",&p[i][j]);
	}
	printf("Enter the second matrix : ");
	for(i=0;i<r2;i++)
	{
		for(j=0;j<c2;j++)
			scanf("%d",&q[i][j]);
	}
	for(i=0;i<r1;i++)
		for(j=0;j<c2;j++)
		{
			for(k=0;k<c1;k++)
				r[i][j]+=(p[i][k]*q[k][j]);
		}
	printf("Result = \n");
	for(i=0;i<r1;i++)
	{
		for(j=0;j<c2;j++)
			printf("%d ",r[i][j]);
		printf("\n");
	}
}
