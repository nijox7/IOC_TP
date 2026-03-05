#pragma once

#include <linux/ioctl.h>

struct cord_xy {
    int line;
    int row;
} cord_xy;

#define IOC_MAGIC 't'
#define LCDIOCT_CLEAR _IO(IOC_MAGIC, 20)
#define LCDIOCT_SETXY _IOW(IOC_MAGIC, 21, struct cord_xy)
