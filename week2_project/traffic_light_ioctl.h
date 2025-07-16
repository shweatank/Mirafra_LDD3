#include <linux/ioctl.h>
#define TL_MAGIC    0xCD
#define TL_SET_RED      _IO(TL_MAGIC, 1)
#define TL_SET_YELLOW   _IO(TL_MAGIC, 2)
#define TL_SET_GREEN    _IO(TL_MAGIC, 3)
#define TL_IRQ_ENABLE   _IO(TL_MAGIC, 4)
#define TL_IRQ_DISABLE  _IO(TL_MAGIC, 5)


