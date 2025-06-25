#include<stdio.h>
int at_oi(char s[])
{
	int i=0;
	int sign=1,result=0;
	if(s[i]=='+'||s[i]=='-')
	{
		if(s[i]=='-')
		{
			sign=-1;
		}
		i++;
	}
	while(s[i])
	{
		if(s[i]<'0'||s[i]>'9')
			return -1;
		result=result*10+(s[i]-'0');
		i++;
	}
	return result*sign;
}
int main()
{
	char str[32];
	printf("Enter a string ");
	scanf("%s",str);
	printf("%d\n",at_oi(str));
	return 0;
}
