#include <stdio.h>

struct Student
{
    char name[50];
    int roll;
    union 
    {
        float marks;
        char grade;
    } result;
};

int main() {
    struct Student s1 = {"Harsha", 1, .result.marks = 85.5};
    struct Student s2 = {"Ravi", 2, .result.grade = 'A'};

    printf("%s %.2f \n", s1.name, s1.result.marks);
    printf("%s %c\n", s2.name, s2.result.grade);

    return 0;
}

