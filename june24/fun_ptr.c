#include<stdio.h>
#include<stdlib.h>
typedef struct
{
        int (*add)(int,int);
        int (*sub)(int,int);
        int (*mul)(int,int);
        float (*div)(int,int);
}calculator;

int Add(int i1,int i2)
{
        return i1+i2;
}
int Sub(int i1,int i2)
{
        return i1-i2;
}
int Mul(int i1,int i2)
{
        return i1*i2;
}
float Div(int i1,int i2)
{
        return i1/i2;
}

calculator ptr={
        .add=Add,
        .sub=Sub,
        .mul=Mul,
        .div=Div
};

int main()
{

        int i1=10,i2=5;
        printf("add:%d\n",ptr.add(i1,i2));
        printf("sub:%d\n",ptr.sub(i1,i2));
        printf("add:%d\n",ptr.mul(i1,i2));
        printf("add:%f\n",ptr.div(i1,i2));
}

