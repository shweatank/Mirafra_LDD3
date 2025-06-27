#include <stdio.h>
#include <stdlib.h>

void printSubset(int* subset, int size) {
    printf("[");
    for (int i = 0; i < size; i++) {
        printf("%d", subset[i]);
        if (i != size - 1)
            printf(",");
    }
    printf("]");
}

void backtrack(int* nums, int numsSize, int* subset, int subsetSize, int index) {
    printSubset(subset, subsetSize);
    printf("\n");

    for (int i = index; i < numsSize; i++) {
        subset[subsetSize] = nums[i];
        backtrack(nums, numsSize, subset, subsetSize + 1, i + 1);
    }
}

void subsets(int* nums, int numsSize) {
    int* subset = (int*)malloc(sizeof(int) * numsSize);
    backtrack(nums, numsSize, subset, 0, 0);
    free(subset);
}

int main() {
    int nums1[] = {1, 2, 3};
    int size1 = sizeof(nums1) / sizeof(nums1[0]);
    subsets(nums1, size1);
    printf("\n");
    return 0;
}

