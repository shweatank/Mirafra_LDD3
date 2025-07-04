#ifndef INT_CALC_H
#define INT_CALC_H

#define MAGIC_NUM 0xF7

#define IOCTL_START_INPUT   _IO(MAGIC_NUM, 1)
#define IOCTL_STOP_INPUT    _IO(MAGIC_NUM, 2)
#define IOCTL_GET_RESULT    _IOR(MAGIC_NUM, 3, int)

#endif

