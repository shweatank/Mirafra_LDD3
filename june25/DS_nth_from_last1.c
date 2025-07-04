#include<stdio.h>
#include<stdlib.h>
typedef struct s
{
        int data;
        struct s *link;
}st;
st *create_node(int d)
{
        st *nu=NULL;
        nu=calloc(1,sizeof(st));
        if(nu==NULL)
        {
                printf("memory allocation failed\n");
                return NULL;
        }
        nu->data=d;
        nu->link=NULL;
        return nu;
}
void print(st *head)
{
        while(head)
        {
                printf("%d ",head->data);
                head=head->link;
        }
        printf("\n");
}
st *nth_from_last(st *head,int n)
{
        int len=1;
        st *temp=head;
        while(temp->link)
        {
                len++;
                temp=temp->link;
        }
        if(n>len || n<=0)
        {
                printf("invalid \n");
                return NULL;
        }
        temp=head;
        n=len-n;
        for(int i=0;i<n;i++)
        {
                temp=temp->link;
        }
        return temp;
}

int main()
{
        st *head=NULL;
        head=create_node(10);
        head->link=create_node(20);
        head->link->link=create_node(30);
        head->link->link->link=create_node(40);
        head->link->link->link->link=create_node(50);
        head->link->link->link->link->link=create_node(60);

        print(head);
        st *temp=nth_from_last(head,2);
        if(temp!=NULL)
        printf("%d\n",temp->data);
}
