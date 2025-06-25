#include <stdio.h>
#include <string.h>

struct student {
    int id;
    char name[50];
    float marks;
};

void data(struct student s[], int n) {
    for (int i = 0; i < n; i++) {
        printf("enter data\n");
        scanf("%d", &s[i].id);
        scanf(" %[^\n]", s[i].name);
        scanf("%f", &s[i].marks);
    }
}

void display(struct student s[], int n) {
    for (int i = 0; i < n; i++) {
        printf("id: %d, name: %s, marks: %.f\n", s[i].id, s[i].name, s[i].marks);
    }
}

struct student topper(struct student s[], int n) {
    int j = 0;
    for (int i = 1; i < n; i++) {
        if (s[i].marks > s[j].marks) {
            j = i;
        }
    }
    return s[j];
}

int isPalindrome(char *str) {
    int l = 0;
    int r = strlen(str) - 1;
    while (l < r) {
        if (str[l] != str[r])
            return 0;
        l++;
        r--;
    }
    return 1;
}

void displayPalindromeNames(struct student s[], int n) {
    printf("\nStudents with palindrome names:\n");
    int found = 0;
    for (int i = 0; i < n; i++) {
        if (isPalindrome(s[i].name)) {
            printf("id: %d, name: %s, marks: %.2f\n", s[i].id, s[i].name, s[i].marks);
            found = 1;
        }
    }
    if (!found)
        printf("No palindrome names found.\n");
}

int main() {
    int n;
    printf("Enter number of students: ");
    scanf("%d", &n);

    struct student s[n];
    data(s, n);

    printf("\nAll students:\n");
    display(s, n);

    struct student top = topper(s, n);
    printf("\nTopper:\n");
    printf("ID: %d, Name: %s, Marks: %.2f\n", top.id, top.name, top.marks);

    displayPalindromeNames(s, n);

    
}

