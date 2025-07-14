#include<stdio.h>
void main()
{
	int num=0,dec=0;
	char bin[32];
	printf("Enter a number ");
	scanf("%d",&num);
	while(num>0)
	{
		int rem=num%8;

