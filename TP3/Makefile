# changer les valeurs par dÃ©faut de RPI et DIR
RPI?=21
DIR?=carbou_das/lab3
APP=lcd_user_xy

CFLAGS=-Wall -Wfatal-errors -O2
LDFLAGS=-static -L$(LIB) -lgpio

KERNELDIR       = /dsk/l1/misc/linux-rpi-3.18.y
CROSSDIR        = /users/enseig/franck/E-IOC
CROSS_COMPILE   = $(CROSSDIR)/arm-bcm2708hardfp-linux-gnueabi/bin/bcm2708hardfp-

all:  $(APP).x upload

$(APP).x: $(APP).c
	$(CROSS_COMPILE)gcc -o $@ $< -I$(INC) -O2 -static

upload: 
	scp -P622$(RPI) $(APP).x pi@peri:$(DIR)

clean:
	rm -f *.o *.x *~
