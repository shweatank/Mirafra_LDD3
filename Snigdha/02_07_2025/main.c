#include <stdio.h>

/*int normal_function(int x) {
    int sum = 0;
    for (int i = 0; i < 100; i++) {
        sum += x + i;
    }
    return sum;
}*/

static inline int inline_function(int x) {
    int sum = 0;
    for (int i = 0; i < 100; i++) {
        sum += x + i;
    }
    return sum;
}

int main() {
	int x=1,y=2,z=3;
    //int a = normal_function(x);
    //int b = normal_function(y);
   // int c = normal_function(z);

    int d = inline_function(x);
    int e = inline_function(y);
    int f = inline_function(z);

    printf("%d %d %d\n", d, e, f);
    return 0;
}

