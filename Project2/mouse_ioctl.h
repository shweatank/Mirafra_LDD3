#ifndef _MOUSE_IOCTL_H_
#define _MOUSE_IOCTL_H_

#include <linux/ioctl.h>

#define MOUSE_IOCTL_MAGIC   'M'
#define IOCTL_GET_COUNT     _IOR(MOUSE_IOCTL_MAGIC, 0, unsigned long)
#define IOCTL_CLR_COUNT     _IO (MOUSE_IOCTL_MAGIC, 1)

#endif /* _MOUSE_IOCTL_H_ */

