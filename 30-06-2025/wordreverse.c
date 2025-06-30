//30 June 2025
//reverse words in a string
#include<stdio.h>
void reverse(char *str,int start,int stop)
{
	char temp;
	while(start<stop)
	{
		temp=str[start];
		str[start]=str[stop];
		str[stop]=temp;
		start++;
		stop--;
	}
}
void wordreverse(char *str)
{
	int len;
	for(len=0;str[len];len++);
	reverse(str,0,len-1);
	int start=0,end=0;
	while(str[end])
	{
		while(start<len && str[start]==' ')
			start++;
		end=start;
		while(str[end]!=' ' && str[end]!='\0')
			end++;
		reverse(str,start,end-1);
		start=end;
	}
	reverse(str,start,end-1);
}
void main()
{
	char str[100];
	printf("Enter the string: ");
	scanf("%[^\n]",str);
	wordreverse(str);
	printf("REsult = %s\n",str);
}
