# TP6 Programmation ESP32 / Arduino 

## Auteurs
- Das
- Carbou

## Pour lancer arduino
Aller dans une. session avec l'os Mate (Fermer la session -> login -> en bas a droite selectionner mate -> mot de passe)
/opt/arduino-1.8.8/arduino
    Dans le texte précédent, quel est le nom de la boîte à lettre et comment est-elle initialisée ?
    Ajouter une tâche nommée lum qui lit toutes les 0,5 seconde le port analogique [...] (par analogRead()) sur lequel se trouve la photo-résistance et qui sort sa valeur dans une boite à lettre. Cette boite à lettre sera connectée à la tâche oled. Vous afficher la valeur en pourcentage de 0% à 100% en utilisant la fonction map()
    Mofifier la tâche Led pour que la fréquence de clignotement soit inversement proportionnel à la lumière reçue (moins il y a de lumière plus elle clignote vite). La tâche Led devra donc se brancher avec la tâche lum avec une nouvelle boite à lettre. Il doit y avoir deux boites sortant de lum, l'une vers oled l'autre vers led. 
## TODO
 Vous rendrez une archive avec :
    un compte rendu au format en pdf (à partir de Markdown ou d'un traitement de texte) expliquant en détail votre sketch Arduino, l'idée est de vous approprier cette méthode de programmation, alors les explications que vous faites sont utiles pour vous. Vous devez aussi insérer un graphe représentant les tâches et leur communication (avec graphviz).
    le sketch Arduino avec des commentaires, celui contenant toutes les tâches. 


## Réponses aux questions du TME
TODO Expliquer pourquoi il est déconseillé d'utiliser des variables static
On utilise pas de variables locales static dans les fonctions step_tache et init_tache pour deux raisons.
La première est que si l'on déclare une variable static dans init_tache, elle sera inaccessible dans step_tache.
La seconde est que l'utilisation d'un variable static dans step_tache, rend cette variable commune à tous les taches.

- Que contient le tableau last_period[] et à quoi sert-il ?

Le tableau last_period[] contient le numéro de la dernière période lors du dernier appel de waitFor pour chaque timer.

- Si on a deux tâches indépendantes avec la même période, pourquoi ne peut-on pas utiliser le même timer dans waitFor() ?

On ne peut pas utiliser le même timer pour deux tâches indépendantes car lors de l'appel waitFor, la valeur dans last_period[timer] est mise à jour.
Seule la première des taches à faire l'appel à waitFor obtiendra le nombre de période écoulée, l'autre obtiendra 0 puisque le tableau static last_period aura été modifié.

- Dans quel cas la fonction waitFor() peut rendre 2 ?

La fonction waitFor() peut rendre 2 lorsque au moins 2 périodes se sont écoulées.



- Dans le texte précédent, quel est le nom de la boîte à lettre et comment est-elle initialisée ?

Le nom de la boîte aux lettre est mb. Elle est initialisée à l'aide d'une structure possèdant l'état de la boîte et sa valeur. Elle est initialement à l'état vide et n'a donc pas de valeur.

- Ajouter une tâche nommée lum qui lit toutes les 0,5 seconde le port analogique [...] (par analogRead()) sur lequel se trouve la photo-résistance et qui sort sa valeur dans une boite à lettre. Cette boite à lettre sera connectée à la tâche oled. Vous afficher la valeur en pourcentage de 0% à 100% en utilisant la fonction map()


- Modifier la tâche Led pour que la fréquence de clignotement soit inversement proportionnel à la lumière reçue (moins il y a de lumière plus elle clignote vite). La tâche Led devra donc se brancher avec la tâche lum avec une nouvelle boite à lettre. Il doit y avoir deux boites sortant de lum, l'une vers oled l'autre vers led. 


## Expériences réalisées
