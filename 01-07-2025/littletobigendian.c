#include<stdio.h>
union u
{
	int num;
	char ch[4];
};
void main()
{
	union u data;
	char t;
	printf("Emter the hexadecimal numebr : ");
	scanf("%x",&data.num);
	t=data.ch[0];
	data.ch[0]=data.ch[3];
	data.ch[3]=t;
	t=data.ch[1];
	data.ch[1]=data.ch[2];
	data.ch[2]=t;
	printf("REsult = %x\n",data.num);
}
