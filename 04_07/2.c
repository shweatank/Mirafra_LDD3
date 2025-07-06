#include <stdio.h>
#include <string.h>

struct one {
    int rollno;
    char name[20];
    float marks;
};

void main() {
    struct one v[5], t;
    int i, ele;
    ele = sizeof(v) / sizeof(v[0]);

    // Input details for each record
    for (i = 0; i < 5; i++) {
        printf("Enter integer (roll number):\n");
        scanf("%d", &v[i].rollno);
        printf("Enter string (name):\n");
        scanf("%s", v[i].name);
        printf("Enter float (marks):\n");
        scanf("%f", &v[i].marks);
    }

    // Print original records
    printf("\nOriginal Data:\n");
    for (i = 0; i < 5; i++) {
        printf("%d  %s  %.2f\n", v[i].rollno, v[i].name, v[i].marks);
    }

    int op, j;
    printf("\nEnter option for sorting:\n");
    printf("Press 1 for roll number\n");
    printf("Press 2 for name\n");
    printf("Press 3 for marks\n");

    scanf("%d", &op);

    // Sorting based on user choice
    if (op == 1) { // Sort by roll number
        for (i = 0; i < ele - 1; i++) {
            for (j = 0; j < ele - 1 - i; j++) {
                if (v[j].rollno > v[j + 1].rollno) {
                    t = v[j];
                    v[j] = v[j + 1];
                    v[j + 1] = t;
                }
            }
        }
    } else if (op == 2) { // Sort by name
        for (i = 0; i < ele - 1; i++) {
            for (j = 0; j < ele - 1 - i; j++) {
                if (strcmp(v[j].name, v[j + 1].name) > 0) {
                    t = v[j];
                    v[j] = v[j + 1];
                    v[j + 1] = t;
                }
            }
        }
    } else if (op == 3) { // Sort by marks
        for (i = 0; i < ele - 1; i++) {
            for (j = 0; j < ele - 1 - i; j++) {
                if (v[j].marks > v[j + 1].marks) { // Correct condition
                    t = v[j];
                    v[j] = v[j + 1];
                    v[j + 1] = t;
                }
            }
        }
    } else {
        printf("Invalid option entered.\n");
        return;
    }

    // Print sorted records
    printf("\nAfter Sorting:\n");
    for (i = 0; i < ele; i++) {
        printf("%d  %s  %.2f\n", v[i].rollno, v[i].name, v[i].marks);
    }
}

