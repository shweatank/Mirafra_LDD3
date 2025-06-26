//26 June 2025
//Sort students according to rollno or name or marks using function pointer
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

struct student
{
	int rollno;
	char name[20];
	float mark;
};

typedef void(*fptr)(struct student *,int);

void sort_by_rollno(struct student *data,int strength)
{
	int i,j;
	struct student temp;
	for(i=0;i<strength;i++)
	{
		for(j=i+1;j<strength;j++)
		{
			if(data[i].rollno>data[j].rollno)
			{
				temp=data[i];
				data[i]=data[j];
				data[j]=temp;
			}
		}
	}
}
void sort_by_name(struct student *data,int strength)
{
	int i,j;
	struct student temp;
	for(i=0;i<strength;i++)
	{
		for(j=i+1;j<strength;j++)
		{
			if(strcmp(data[i].name,data[j].name)>0)
			{
				temp=data[i];
				data[i]=data[j];
				data[j]=temp;
			}
		}
	}
}
void sort_by_mark(struct student *data,int strength)
{
	int i,j;
	struct student temp;
	for(i=0;i<strength;i++)
	{
		for(j=i+1;j<strength;j++)
		{
			if(data[i].mark>data[j].mark)
			{
				temp=data[i];
				data[i]=data[j];
				data[j]=temp;
			}
		}
	}
}
void main()
{
	int num,i,op;
	fptr fp;
	printf("enter the number of students : ");
	scanf("%d",&num);
	struct student *data=(struct student *)malloc(sizeof(struct student));
	for(i=0;i<num;i++)
		scanf("%d%s%f",&data[i].rollno,data[i].name,&data[i].mark);
	printf("Enter 1 for sort by rollno	2 for sort by name 	3 for sort by mark : ");
	scanf("%d",&op);
	switch(op)
	{
		case 1:fp=sort_by_rollno;
		       fp(data,num);
		       break;
		case 2:fp=sort_by_name;
		       fp(data,num);
		       break;
		case 3:fp=sort_by_mark;
		       fp(data,num);
		       break;
		default:printf("Invalid choice...\n");
			return;
	}
	printf("Result  : \n");
	for(i=0;i<num;i++)
		printf("%d  %s  %f\n",data[i].rollno,data[i].name,data[i].mark);
}
