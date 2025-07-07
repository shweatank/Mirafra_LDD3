// File: vmem_ioctl.h
#ifndef VMEM_IOCTL_H
#define VMEM_IOCTL_H

#include <linux/ioctl.h>

#define VMEM_IOCTL_BASE     'M'

#define IOCTL_SET_MEM_SIZE  _IOW(VMEM_IOCTL_BASE, 1, size_t)
#define IOCTL_CLEAR_MEM     _IO(VMEM_IOCTL_BASE, 2)
#define IOCTL_GET_STATS     _IOR(VMEM_IOCTL_BASE, 3, struct vmem_stats)

struct vmem_stats {
    size_t total_size;
    size_t bytes_read;
    size_t bytes_written;
};

#endif

