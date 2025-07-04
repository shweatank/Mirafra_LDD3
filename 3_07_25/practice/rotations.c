#include<stdio.h>
void main()
{
    int arr[]={1,2,3,4,5,6,7,8,9};
    int len=sizeof(arr)/sizeof(arr[0]);
    int k,i,j;
    printf("Enter k value ");
    scanf("%d",&k);
    while(k>0)
    {
        i=len-1;
        int a=arr[i];
        for(j=i-1;j>=0;j--)
        {
            arr[j+1]=arr[j];
        }
        arr[j+1]=a;
        k--;
    }
    printf("After rotation ");
    for(i=0;i<len;i++)
    {
        printf("%d ",arr[i]);
    }
    printf("\n");
}
