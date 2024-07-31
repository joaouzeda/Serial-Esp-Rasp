import RPi.GPIO as GPIO
import paho.mqtt.client as paho
from paho import mqtt
import time 
import Serial

Ser = serial.Serial("/dev/ttyS0", 115200)
client = paho.Client(client_id="", userdata=None, protocol=paho.MQTTv5)
client.tls_set(tls_version=mqtt.client.ssl.PROTOCOL_TLS)
client.username_pw_set("username", "password")
client.connect("14b5793c334743769b3e9fb1e4008401.s2.eu.hivemq.cloud", 8883)
client.connect("ssl://localhost", 8883)
#client.publish("encyclopedia/temperature", payload="hot", qos=1)

topic = "teste/serial read"

GPIO.setwarnings(False)

print("Iniciando")

while True:
    time.sleep(0.5)
    teste = Ser.readline()
    client.publish(topic, payload = teste, qos = 1)
    time.sleep(0.5)