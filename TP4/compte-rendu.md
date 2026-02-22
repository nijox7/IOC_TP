# Compte-rendu TP4

## 1. Communication par FIFO

Pour échanger dans un pipe fifo créer par mkfifo, cela nécessite d'avoir une ouverture en lecture et en écriture simultanément sinon l'écriture et la lecture sont bloquées.
Il faut donc un lecteur et un écrivain ayant ouvert le fichier pour y échanger des données.

## 2.
En python2 utiliser: `raw_input` et pas `input`.
