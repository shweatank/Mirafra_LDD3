//user
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>     // pr_info(), pr_err(), etc.


int main(void)
{
    int fd = open("/dev/simple_char_dev", O_WRONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    day_t today = MONDAY;               /* choose the value to send */

    ssize_t n = write(fd, &today, sizeof(today));
    if (n != sizeof(today))
        perror("write");
    else
        printf("Sent enum value %d (MONDAY)\n", today);

    close(fd);
    return 0;
}


