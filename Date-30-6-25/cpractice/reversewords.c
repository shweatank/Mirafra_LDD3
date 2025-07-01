#include<stdio.h>
#include<string.h>
int main()
{
	char str[100];
	printf("Enter The String:\n");
	scanf(" %[^\n]",str);

	int len=strlen(str);
	int start,end,i,j;

	for (i = 0, j = len - 1; i < j; i++, j--) 
	{
        	char temp = str[i];
        	str[i] = str[j];
        	str[j] = temp;
    	}
	
    	i = 0;
    	while (i < len) 
	{
       		while (str[i] == ' ')
            	i++;
        	start = i;
        	while (i < len && str[i] != ' ')
            	i++;
        	end = i - 1;
        	while (start < end)
	       	{
            		char temp = str[start];
            		str[start] = str[end];
            		str[end] = temp;
            		start++;
            		end--;
        	}
    	}

    printf("Reversed words: %s\n", str);
    return 0;
}


