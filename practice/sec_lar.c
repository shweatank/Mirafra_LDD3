#include<stdio.h>
void main(){
	int arr[7]={2,3,1,4,5,9,9};
	int max1=arr[0],max2=-1,i;
	for(i=0;i<7;i++){
		if(arr[i]>max1){
			max2=max1;
			max1=arr[i];
		}
		else if(arr[i]>max2 && arr[i]!=max1)
		max2=arr[i];
	}
	printf("%d %d\n",max1,max2);
}

//int min1=arr[0],min2=1e9;  In C, 1e9 is a floating-point literal written in scientific (exponential) notation. It represents the number:

//1 × 10⁹ = 1,000,000,000
