#include<stdio.h>
#include<string.h>

void reverse(char str[],int start,int end)
{
	if(start>=end)
		return;

	char temp=str[start];
	str[start]=str[end];
	str[end]=temp;

	reverse(str,start+1,end-1);
}
int main()
{
	char str[30];
	printf("Enter a string:");
	scanf("%s",str);

	int len=strlen(str);
	reverse(str,0,len-1);

	printf("Reversed string is:%s\n",str);
	return 0;
}
