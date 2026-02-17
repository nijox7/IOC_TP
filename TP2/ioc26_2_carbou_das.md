# TP2 Driver Linux sur RaspberryPi

## Auteurs
- Carbou
- Das

## Réponses aux questions du TME

- Quelle fonction est exécutée lorsqu'on insère le module du noyau ?
\- La fonction indiquée en paramètre dans la fonction `module_init()` (dans le contexte de la question c'était `mon_module_init`)

- Quelle fonction est exécutée lorsqu'on enlève le module du noyau ?
\- La fonction indiquée en paramètre dans la fonction `module_exit()` (dans le contexte de la question c'était `mon_module_cleanup`)

- Comment voir que le paramètre a bien été lu ? (paramètre de module)
\- On peut faire un `printk` (dans le module_init) affichant la valeur de la variable (ou des variables dans le cas d'un tableau) puis vérifier dans `dmesg` si la valeur est correcte.

- Comment savoir que le device a été créé ?
\- On peut vérifier si le dossier `/dev/` contient notre driver (`ls /dev/ | grep nom_du_driver`), on peut vérifier que le module existe bien dans `lsmod`, et que notre major existe bien dans le fichier `/proc/devices`.

- Expliquer comment `insdev` récupère le numéro `major`.
\- Dans insdev on retrouve la ligne : `major=$(awk "\$2==\"$module\" {print \$1;exit}" /proc/devices)`, c'est la ligne dans laquelle on récupère le major.
On voit qu'on utilise `awk`, qui va découper le fichier `/proc/devices` en colonne, on compare ensuite le deuxième élément d'une ligne avec le nom de notre module, et si les deux noms sont égaux, on affiche l'élément 1 de la ligne (c'est-à-dire le major). Puis si l'élément est trouvé on `exit` (pour éviter de continuer la recherche).


## Expériences réalisées

### Insertion et Retraite d'un Module

- Ajout d'un module
On ajoute le module à l'aide de `insmod`, il est ensuite visible dans la liste des modules inséré (`lsmod`).

- Affichage des messages noyau (`dmesg`)
`dmesg` permet d'afficher les messages du noyau, dont les `printk` qu'on met dans notre code d'`open, read, write, et close` du module.
Lors du premier `dmesg` on obtient l'affichage de "Hello World <Das Carbou> !"
(qui est le message print dans la fonction `mon_module_init()`, qui est la fonction exécutée lors de l'insertion du module dans le noyau).
    
- Retraite d'un module
On retire le module du noyau à l'aide de `rmmod`, il est ensuite supprimé de la liste de module (`lsmod`).
Dans notre cas, le prochain `dmesg` affichera "Goodbye World <Das Carbou> !" (qui est le message print dans notre fonction mon_module_cleanup, et qui est la fonction appelée lorsqu'on enlève le module du noyau).
    
### Création d'un driver

- Fonctions de comportement du pilote
On ajoute des fonctions de pilotage, pouvant être utilisées par un processus pour piloter un périphérique. On doit ensuite créer la struct file_operation fops qui contiendra la liste de nos fonctions de pilotage, en les liant à des primitives imposées (comme open, read, write et release).
    
- Enregistrement du driver
On enregistre le driver à l'aide de `register_chrdev` (qui renvoie le major) et on y indique la structure qui contient nos primitives (nos fonctions de pilotage), le nom du driver et la valeur du major (ou 0 s'il est sélectionné par le système).
On décharge le driver à l'aide de `unregister_chrdev`.

    
- Création du noeud dans `/dev`
Pour utiliser le driver, on doit ensuite créer un noeud dans `/dev` (`mknod [name] [type] [major] [minor]`, ex : `mknod "led" c 246 0`). Pour récupérer le major de notre driver, on peut regarder dans le fichier `/proc/devices`. (le minor est un numéro d'instance, on le met à 0 ici)
    
- Utilisation du driver
On peut utiliser le driver avec `echo` ou `dd`.
Lorsqu'on fait les appels à `echo` et `dd` sur le driver, on peut voir dans le debug (`dmesg`) qu'à l'exécution de ces 2 commandes on fait une suite de `open - write - close` (pour `echo`) et `open - read - close` (pour `dd`).
On peut deviner que les programmes `echo` et `dd` font des `open, read, write, close` de base, et que notre driver accepte ces fonctions (fonctions de pilotage).

- Fonctions read et write
On peut modifier les fonctions read et write pour agir sur les leds par exemple. Pour cela on doit réutiliser la structure des registres gpio, et les utiliser (comme dans le TP1).
