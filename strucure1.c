//24 June 2025
////Program to read n students' data and find students with highest avg marks
#include<stdio.h>
#include<stdlib.h>
struct student
{
	int rollno;
	char name[20];
	int mark[3];
	float avg;
};
void main()
{
	int num,i;
	float max=0;
	printf("Enter the number of students: ");
	scanf("%d",&num);
	struct student *datasheet=(struct student *)malloc(sizeof(struct student)*num);
	for(i=0;i<num;i++)
	{
		printf("Enter the %d th student details : ",i);
		scanf("%d%s%d%d%d",&datasheet[i].rollno,datasheet[i].name,&datasheet[i].mark[0],&datasheet[i].mark[1],&datasheet[i].mark[2]);
	}
	for(i=0;i<num;i++)
	{
		datasheet[i].avg=((float)(datasheet[i].mark[0]+datasheet[i].mark[1]+datasheet[i].mark[2]))/3;
		if(max<datasheet[i].avg)
			max=datasheet[i].avg;
	}
	printf("Students with highest avg marks : \n");
	for(i=0;i<num;i++)
	{
		if(datasheet[i].avg==max)
		{
			printf("Rollno = %d\nName = %s\n",datasheet[i].rollno,datasheet[i].name);
		}
	}
	free(datasheet);
}
