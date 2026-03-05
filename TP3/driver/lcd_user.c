#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "ioctl.h"

int main() {
    int fd = open("/dev/lcd_CD", O_RDWR);

    struct cord_xy c;
    c.line = 1;
    c.row = 0;

    if(ioctl(fd, LCDIOCT_CLEAR)) {
        perror("ioctl");
        return -1;
    }

    if(ioctl(fd, LCDIOCT_SETXY, &c)) {
        perror("ioctl");
        return -1;
    }
    
    free(buf);
    return 0;
}
