#include <stdio.h>

int main() {
    int i = 0;
    while(i != 10) {  // Bug: i is never incremented
        printf("Looping...\n");
    }
    return 0;
}
//answer
//int i=0;
//whie(i!=10)
//{
//i++;
//printf("looping...\n");
//}

