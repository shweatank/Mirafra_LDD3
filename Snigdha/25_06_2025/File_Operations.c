#include <stdio.h>
#include <stdlib.h>

typedef struct {
    FILE *(*open)(const char *filename, const char *mode);
    void (*read)(FILE *fp);
    void (*write)(FILE *fp);
    void (*close)(FILE *fp);
} FileOperations;


FILE *my_open(const char *filename, const char *mode) {
    FILE *fp = fopen(filename, mode);
    if (fp == NULL) {
        printf("Error: Cannot open file %s\n", filename);
        exit(1);
    }
    return fp;
}

void my_write(FILE *fp) {
    int ch;
    printf("Enter text (Ctrl+D to end):\n");
    while ((ch = getchar()) != EOF) {
        fputc(ch, fp);
    }
    printf("Writing Done\n");
}

void my_read(FILE *fp) {
    int ch;
    printf("\nReading from file:\n");
    while ((ch = fgetc(fp)) != EOF) {
        putchar(ch);
    }
}

void my_close(FILE *fp) {
    fclose(fp);
}

FileOperations fileOps = {
    .open = my_open,
    .write = my_write,
    .read = my_read,
    .close = my_close
};

int main() {
    FILE *fptr = fileOps.open("file1.txt", "w");
    fileOps.write(fptr);
    fileOps.close(fptr);

    FILE *fread = fileOps.open("file1.txt", "r");
    fileOps.read(fread);
    fileOps.close(fread);

    return 0;
}

