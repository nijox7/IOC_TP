#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("JSP");
MODULE_AUTHOR("Carbou, Das, 2026");
MODULE_DESCRIPTION("Module, aussitot insere, aussitot efface");

static int btn;
module_param(btn, int, 0); // on ajoute un paramètre au module (btn=18 par exemple)
MODULE_PARM_DESC(btn, "numéro du port du bouton");

static int __init mon_module_init(void) // fonction appelée lors de l'insertion du module (insmod)
{
    printk(KERN_DEBUG "Hello World <Das Carbou> !\n");
    printk(KERN_DEBUG "btn=%d !\n", btn);
    return 0;
}


static void __exit mon_module_cleanup(void) // fonction appelée lors de la supression du module (rmmod)
{
    printk(KERN_DEBUG "Goodbye World <Das Carbou> !\n");
}

module_init(mon_module_init);
module_exit(mon_module_cleanup);

/*
Question 1: la fonction module_init est appelée lors de l'insertion
Question 2: la fonction module_exit est appelée lors de la suppresion du module
*/
