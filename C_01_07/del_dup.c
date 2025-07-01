#include<stdio.h>
int remove_dup(int *arr,int n){
	int i,j,dup,k=0;
	for(i=0;i<n;i++){
		dup=0;
		for(j=0;j<k;j++){
			if(arr[i]==arr[j]){
				dup=1;
				break;
			}
		}
		if(dup==0)
			arr[k++]=arr[i];
	}
	return k;
}


int main() {
    int a[] = {3, 3, 2, 4, 4, 1, 2, 3, 7, 9};
    int n = sizeof(a) / sizeof(a[0]);

    n = remove_dup(a, n);

    printf("Array after removing duplicates: ");
    for (int i = 0; i < n; i++) {
        printf("%d ", a[i]);
    }
    printf("\n");
}
