#include<stdio.h>
#include<string.h>
struct stu
{
   int roll;
   char name[10];
};
int main()
{
   struct stu e;
   e.roll=10;
   strcpy(e.name,"njdo");
   struct stu *ptr=&e;//address is imp
   printf("%d  %s\n",ptr->roll,ptr->name);
}
   
