#include<stdio.h>
struct emp
{
 int id;
 char name[20];
};
int main()
{
    struct emp e[3];
    for(int i=0;i<3;i++)
    {
       printf("enetr the %d emp details:\n",i+1);
       scanf("%d",&e[i].id);
       getchar();    
       scanf("%s",e[i].name);
    }
    for(int i=0;i<3;i++)
    {
       printf("%d  %s\n",e[i].id,e[i].name);
    }
}
