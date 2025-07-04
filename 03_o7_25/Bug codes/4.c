#include <stdio.h>

int main() {
    int arr[5] = {0};
    for (int i = 0; i <= 5; i++) {  // Bug: i <= 5, out-of-bounds access
        arr[i] = i;
    }
 for(int i=0;i<5;i++)
	 printf("%d ",arr[i]);
    return 0;
}
//answer
//in for loop
//for(int i=0;i<5;i++)
