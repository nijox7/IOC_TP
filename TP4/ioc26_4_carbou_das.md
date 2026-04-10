---
title: 'TP1 Serveur WEB minimaliste'

---

# TP4 Installation, "Hello World" et led-bp user

## Auteurs
- Das
- Carbou

## Réponses aux questions du TME

- Dans quel répertoire est créée la fifo?
> La fifo est créée dans le répertoire */tmp/*.

- Quelle est la différence entre mkfifo et open?
> La différence entre mkfifo et open est que mkfifo créer un fichier temporaire qui disparaît lorsqu'il n'y a plus d'écrivain et de lecteur alors que open ouvre un fichier en lecture ou en écriture ou les deux.

- Pourquoi tester que la fifo existe?
> Dans le cas où il existe déjà on ne doit pas le recréer.

- À quoi sert flush?
> *flush* permet de vider le buffer d'écriture, et donc d'effectuer l'écriture dans le fichier.

- Pourquoi ne ferme-t-on pas la fifo?
> Car python le ferme automatiquement.

- Que fait readline ?
> Lit une ligne sur le fichier /tmp/myfifo en s'arrêtant donc au premier "\n" rencontré.

- Expliquez pourquoi lorsque l'on un écrivain seul rien ne se passe.
> Lorsque l'on lance un écrivain rien ne se passe sans lecteur car l'écrivain attend qu'il y ait au moins un lecteur pour écrire.

## Expériences réalisées
Voici les retours de nos expériences lors du TP:


## Notions apprises
Durant ce TP nous avons appris de nombreuses notions que nous allons expliquer ici: