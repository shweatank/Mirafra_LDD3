// Online C compiler to run C program online
#include <stdio.h>
struct student {
    int a;
    char b;
    float c;
};
int main() {
    // Write C code here
    printf("Try programiz.pro");
    struct student a[2];
    printf("\n%ld\n",(char *)&a[1]-(char *)&a[0]);

    return 0;
}
