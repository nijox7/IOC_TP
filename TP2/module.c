#define MODULE
#include <linux/module.h>
#include <linux/init.h>

static int __init module_init() {
    printk(KERN_DEBUG "Hello World <Das Carbou> !\n");
    return 0;
}

static int __init module_exit() {
    printk(KERN_DEBUG "Goodbye World <Das Carbou> !\n");
    return 0;
}

module_init(module_init);
module_exit(module_exit);
o

/*
Question 1: la fonction module_init est appelée lors de l'insertion
Question 2: la fonction module_exit est appelée lors de la suppresion du module

*/