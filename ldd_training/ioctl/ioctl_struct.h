#ifndef IOCTL_STRUCT_H
#define IOCTL_STRUCT_H

#include <linux/ioctl.h>

#define MAGIC_NUM 100

struct my_data
{
    int val1;
    int val2;
};
#define IOCTL_WRITE_DATA _IOW(MAGIC_NUM, 1, struct my_data)
#define IOCTL_READ_DATA  _IOR(MAGIC_NUM, 2, struct my_data)

#endif

