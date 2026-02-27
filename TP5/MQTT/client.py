import paho.mqtt.client as mqtt
import sys

def on_connect(client, userdata, flags, rc): # Callback connexion broker
    print("Connecté " + str(rc))
    client.subscribe("vote")

def on_message(client, userdata, msg): # Callback subscriber
    print("Message reçu : {msg.topic} {str(msg.payload)}")
    exit(0)

client = mqtt.Client() # Création d'une instance client
client.on_connect = on_connect # Assignation des callbacks
client.on_message = on_message
client.connect("132.227.67.61", int(sys.argv[1]), 60) # Connexion au broker
client.publish("vote", sys.argv[2] + ":" + sys.argv[3]) # Publication d'un message
client.loop_forever() # pour attendre les messages
