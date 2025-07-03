#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define DEVICE "/dev/read_write_char_dev"
#define BUF_SIZE 128

// Function to reverse a string
void to_rev(char *str)
{
    int i = 0, j = strlen(str) - 1;
    while (i < j)
    {
        char temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

// Converts integer to string
void num_string(int num, char *str)
{
    int i = 0, flag = 0;
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    if (num < 0)
    {
        str[i++] = '-';
        num = -num;
        flag = 1;
    }

    int start = i; 
    while (num)
    {
        str[i++] = (num % 10) + '0';
        num /= 10;
    }

    str[i] = '\0';

    if (flag)
        to_rev(str + 1); 
    else
        to_rev(str + start);
}

// Converts string to integer
int string_int(const char *str)
{
    int res = 0, sign = 1;

    if (*str == '-')
    {
        sign = -1;
        str++;
    }

    while (*str)
    {
        if (*str >= '0' && *str <= '9')
            res = res * 10 + (*str - '0');
        else
            break; 
        str++;
    }

    return sign * res;
}

int main()
{
    int num;
    char write_buf[BUF_SIZE];
    char read_buf[BUF_SIZE];

    int fd = open(DEVICE, O_RDWR);
    if (fd < 0)
    {
        perror("Failed to open device");
        return 1;
    }

    printf("Enter an integer: ");
    scanf("%d", &num);

    // Convert integer to string and write to device
    num_string(num, write_buf);
    ssize_t written = write(fd, write_buf, strlen(write_buf));
    if (written < 0)
    {
        perror("Failed to write to device");
        close(fd);
        return 1;
    }
    printf("Written %zd bytes: %s\n", written, write_buf);

    // Read back the string
    lseek(fd, 0, SEEK_SET);
    ssize_t read_bytes = read(fd, read_buf, BUF_SIZE - 1);
    if (read_bytes < 0)
    {
        perror("Failed to read from device");
        close(fd);
        return 1;
    }

    read_buf[read_bytes] = '\0';

    // Convert reversed string back to integer
    int reversed_num = string_int(read_buf);
    printf("Read reversed number: %d\n", reversed_num);

    close(fd);
    return 0;
}

