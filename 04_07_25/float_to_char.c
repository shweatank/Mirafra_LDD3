#include<stdio.h>
void reverse(char *str, int len) {
    int i = 0, j = len - 1;
    while(i < j) {
        char temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}
int main()
{
	float num;
	int temp,n,ndp,i=0,j=0;
	char arr[10];
	printf("enter the float  num:\n");
	scanf("%f",&num);
	printf("enter the number of decimal points\n");
	scanf("%d",&ndp);
	temp=num;
	n=num;
	while(n)
	{
		arr[i]=(n%10)+48;
		n=n/10;
		i++;
	}
        reverse(arr,i);
	arr[i]='.';
	i++;
	for(j=0;j<=ndp;j++)
	{
		num=(num-temp)*10;
		temp=num;
		arr[i]=temp+48;
		i++;
	}
	reverse(arr,i);
	for(--i;i>=0;i--)
	{
		printf("%c ",arr[i]);
	}
}



