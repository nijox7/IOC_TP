import paho.mqtt.client as mqtt
import sys

votes = {}

def on_connect(client, userdata, flags, rc): # Callback connexion broker
    print("Connecté " + str(rc))
    client.subscribe("vote")

def on_message(client, userdata, msg): # Callback subscriber
    print("Message reçu : {msg.topic} {str(msg.payload)}", {msg.topic}, {str(msg.payload)})
    chaine = msg.topic.split(":")
    if(chaine[0] in votes):
        client.publish("vote", "Déjà voté")
    else:
        votes[chaine[0]] = chaine[1]
        client.publish("vote", "A voté")       

def print_votes():
    hiver, ete, total = 0
    for nom in votes:
        print(nom + ":" + votes[nom])
        total += 1
        if votes[nom] == ete:
            ete += 1
        elif votes[nom] == hiver:
            hiver += 1
    print("État des votes actuel:")
    print("Été:" + ete + " Hiver:"+ hiver)
    print("Total des votes:" + total)

client = mqtt.Client() # Création d'une instance client
client.on_connect = on_connect # Assignation des callbacks
client.on_message = on_message
client.connect("132.227.67.61", int(sys.argv[1]), 60) # Connexion au broker
client.loop_forever() # pour attendre les messages
