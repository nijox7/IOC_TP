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
    printk(KERN_DEBUG "Hello World !\n");
    printk(KERN_DEBUG "btn=%d !\n", btn);
    return 0;
}


static void __exit mon_module_cleanup(void) // fonction appelée lors de la supression du module (rmmod)
{
   printk(KERN_DEBUG "Goodbye World!\n");
}

module_init(mon_module_init);
module_exit(mon_module_cleanup);

/*
Q1 : module_init
Q2 : module_exit

Ecrire dans CR ce qu'il se passe lors du chargement et déchargement du module

Etape 2 :
Voir Paramètre lu : printk dans init affiche valeur lue de btn (puis dmesg)
*/
