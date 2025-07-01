#include<stdio.h>

void main() {
    int a[10] = {3, 3, 2, 4, 4, 2, 5, 3, 4, 9};
    int i, j, ele, c, d;
    ele=sizeof(a)/sizeof(a[0]);
    for(i=0;i<ele;i++){
    	c=0,d=0;
    	for(j=0;j<i;j++){
    		if(a[i]==a[j]){
    		c=1;
    		break;
    	}
    	}
    	if(c==1)
    	continue;
    	for(j=0;j<ele;j++){
    		if(a[i]==a[j])
    			d++;
    	}
    	if(d>1)
    	printf("%d %d\n",a[i],d);
    	
  }
}

/*
#include <stdio.h>

int main() {
    int a[] = {1, 1, 2, 2, 3, 3, 4, -1, -1, -2, -2, 4};
    int ele = sizeof(a) / sizeof(a[0]);
    int minVal = a[0], maxVal = a[0];
    for (int i = 1; i < ele; i++) {
        if (a[i] < minVal)
         minVal = a[i];
        if (a[i] > maxVal) 
        maxVal = a[i];
    }

    int range = maxVal - minVal + 1;
    int freq[range];      
    int diff[ele];  
    int count = 0;

    for (int i = 0; i < range; i++)
        freq[i] = 0;

    for (int i = 0; i < ele; i++) {
        int idx = a[i] - minVal;
        freq[idx]++;
        if (freq[idx] == 1) {
            diff[count++] = a[i];
        }
    }
    printf("Duplicates and their counts:\n");
    for (int i = 0; i < count; i++) {
        int idx = diff[i] - minVal;
        if (freq[idx] > 1)
            printf("%d -> %d times\n", diff[i], freq[idx]);
    }

    return 0;
}
*/
    	
