#include <stdio.h>

struct Address {
    char city[20];
    int pincode;
};

struct Student {
    char name[30];
    int roll;
    struct Address addr;
};

int main() {
    struct Student s;

    printf("Enter name, roll, city, and pincode:\n");
    scanf("%s %d %s %d", s.name, &s.roll, s.addr.city, &s.addr.pincode);

    printf("Student Info:\nName: %s\nRoll: %d\nCity: %s\nPincode: %d\n",
           s.name, s.roll, s.addr.city, s.addr.pincode);

    return 0;
}

