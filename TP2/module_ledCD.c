#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

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
    return 0;
}

static ssize_t 
read_led_CD(struct file *file, char *buf, size_t count, loff_t *ppos) {
    printk(KERN_DEBUG "read()\n");
    return count;
}

static ssize_t 
write_led_CD(struct file *file, const char *buf, size_t count, loff_t *ppos) {
    printk(KERN_DEBUG "write()\n");
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
    major = register_chrdev(0, "module_ledCD", &fops_led); // définition d'une valeur de major (valeur sélectionnée automatiquement par le système car on donne 0)
    return 0;
}


static void __exit led_module_cleanup(void) // fonction appelée lors de la supression du module (rmmod)
{
   printk(KERN_DEBUG "Goodbye World!\n");
   unregister_chrdev(major, "module_ledCD"); // on décharge le driver
}

module_init(led_module_init);
module_exit(led_module_cleanup);

/*
Major = 246

$ dd bs=1 count=1 < /dev/module_ledCD
1+0 records in
1+0 records out
1 byte (1 B) copied, 0.00101495 s, 1.0 kB/s

*/
