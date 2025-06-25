//Missing number in a array
#include<stdio.h>
int missingnum(int *arr,int n)
{
int expectedsum=n*(n+1)/2;
int actualsum=0;
for(int i=0;i<n;i++)
	actualsum+=arr[i];
return expectedsum-actualsum;

}
int main()
{
	int arr[]={2,3,1,0};
	int size=sizeof(arr)/sizeof(arr[0]);
	int result=missingnum(arr,size);
	printf("Missing number=%d\n",result);
}
