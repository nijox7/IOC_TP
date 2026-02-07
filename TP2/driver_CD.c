#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <mach/platform.h>


/* ------ ACCES GPIO ------ */

static const int READING = 0;
static const int WRITING = 1;
static const int LED0 = 4;
static const int LED1 = 17;
static const int BP = 18;

struct gpio_s
{
    uint32_t gpfsel[7];
    uint32_t gpset[3];
    uint32_t gpclr[3];
    uint32_t gplev[3];
    uint32_t gpeds[3];
    uint32_t gpren[3];
    uint32_t gpfen[3];
    uint32_t gphen[3];
    uint32_t gplen[3];
    uint32_t gparen[3];
    uint32_t gpafen[3];
    uint32_t gppud[1];
    uint32_t gppudclk[3];
    uint32_t test[1];
}

volatile *gpio_regs = (struct gpio_s *)__io_address(GPIO_BASE);

static void gpio_fsel(int pin, int fun)
{
    uint32_t reg = pin / 10;
    uint32_t bit = (pin % 10) * 3;
    uint32_t mask = 0b111 << bit;
    gpio_regs->gpfsel[reg] = (gpio_regs->gpfsel[reg] & ~mask) | ((fun << bit) & mask);
}

static void gpio_write(int pin, bool val)
{
    printk("pin = %i, val = %i\n", pin, val);
    if (val)
        gpio_regs->gpset[pin / 32] = (1 << (pin % 32));
    else
        gpio_regs->gpclr[pin / 32] = (1 << (pin % 32));
}

static int gpio_read(int pin)
{
    return (gpio_regs->gplev[pin/32] & (1 << (pin % 32))) != 0;
}


/* -------- MODULE -------- */


/* MAJOR */
static int major;

MODULE_LICENSE("JSP");
MODULE_AUTHOR("Carbou, Das, 2026");
MODULE_DESCRIPTION("Driver LED");

/* PARAMETRE BOUTON */
static int btn;
module_param(btn, int, 0); // on ajoute un paramètre au module (btn=18 par exemple)
MODULE_PARM_DESC(btn, "numéro du port du bouton");

/* PARAMETRE LEDS */
#define NBMAX_LED 32
static int leds[NBMAX_LED];
static int nbled;
module_param_array(leds, int, &nbled, 0);
MODULE_PARM_DESC(LEDS, "tableau des numéros de port LED");

/* PRINCIPALES OPERATIONS */

static int 
open_led_CD(struct inode *inode, struct file *file) {
    printk(KERN_DEBUG "open()\n");

    // on set le bouton poussoir en lecture, et les leds en écriture
    gpio_fsel(BP, READING);
    gpio_fsel(LED0, WRITING);
    gpio_fsel(LED1, WRITING);

    return 0;
}

static ssize_t 
read_led_CD(struct file *file, char *buf, size_t count, loff_t *ppos) {
    printk(KERN_DEBUG "read()\n");

    int value = gpio_read(BP); // on lit la valeur du bouton
    printk(KERN_DEBUG "   valeur du BP : %d\n", value);

    return value;
}

static ssize_t 
write_led_CD(struct file *file, const char *buf, size_t count, loff_t *ppos) {
    printk(KERN_DEBUG "write()\n");
    printk(KERN_DEBUG "   buf[0] = %c, buf[1] = %c, count = %i\n", buf[0], buf[1], count);
    // je suppose que buf contient : "XY" X : valeur led0, Y : valeur led1 (par exemple 01 : led 1 éteinte, led2 allumée)

    if(buf[0] == '0') gpio_write(LED0, 0);
    else gpio_write(LED0, 1);
    if(buf[1] == '0') gpio_write(LED1, 0);
    else gpio_write(LED1, 1);
    printk(KERN_DEBUG "   led0=%d, led1=%d\n", gpio_read(LED0), gpio_read(LED1));
    return count;
}

static int 
release_led_CD(struct inode *inode, struct file *file) {
    printk(KERN_DEBUG "close()\n");
    return 0;
}

struct file_operations fops_led =
{
    .open       = open_led_CD,
    .read       = read_led_CD,
    .write      = write_led_CD,
    .release    = release_led_CD 
};

static int __init led_module_init(void) // fonction appelée lors de l'insertion du module (insmod)
{
    printk(KERN_DEBUG "Hello World !\n");
    printk(KERN_DEBUG "btn=%d !\n", btn);
    int i = 0;
    for(; i < nbled; i++)
        printk(KERN_DEBUG "LED %d = %d\n", i, leds[i]);
    major = register_chrdev(0, "driver_CD", &fops_led); // définition d'une valeur de major (valeur sélectionnée automatiquement par le système car on donne 0)
    return 0;
}


static void __exit led_module_cleanup(void) // fonction appelée lors de la supression du module (rmmod)
{
   printk(KERN_DEBUG "Goodbye World!\n");
   unregister_chrdev(major, "driver_CD"); // on décharge le driver
}

module_init(led_module_init);
module_exit(led_module_cleanup);
