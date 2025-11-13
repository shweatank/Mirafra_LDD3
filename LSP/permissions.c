#include <stdio.h>      // printf, perror
#include <sys/stat.h>   // stat
#include <stdlib.h>     // exit

int main() {
    struct stat st;

    // Get metadata for "test.txt"
    if (stat("test.txt", &st) == -1) {
        perror("stat failed");
        exit(1);
    }

    printf("File permissions for test.txt:\n");
    printf("User: %c%c%c\n",
           (st.st_mode & S_IRUSR) ? 'r' : '-',
           (st.st_mode & S_IWUSR) ? 'w' : '-',
           (st.st_mode & S_IXUSR) ? 'x' : '-');

    printf("Group: %c%c%c\n",
           (st.st_mode & S_IRGRP) ? 'r' : '-',
           (st.st_mode & S_IWGRP) ? 'w' : '-',
           (st.st_mode & S_IXGRP) ? 'x' : '-');

    printf("Others: %c%c%c\n",
           (st.st_mode & S_IROTH) ? 'r' : '-',
           (st.st_mode & S_IWOTH) ? 'w' : '-',
           (st.st_mode & S_IXOTH) ? 'x' : '-');

    return 0;
}

