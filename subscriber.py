"""Subscriber code to receive data using PahoMQTT."""
import paho.mqtt.client as mqttClient
import time


def on_connect(client, userdata, flags, rc):
    """on_connect is used to check connection to broker."""
    if rc == 0:

        print("Connected to broker: " + str(broker_address))
        global Connected  # Use global variable
        Connected = True  # Signal connection

    else:
        print("Connection failed to " + str(broker_address))


def on_message(client, userdata, message):
    print("Message received: " + str(message.payload.decode("utf-8")))


Connected = False  # global variable for the state of the connection

broker_address = "192.168.0.28"  # Broker address
port = 1883  # Broker port
user = "astr1x"  # Connection username
password = "astr1x2096"  # Connection password

client = mqttClient.Client("Python")  # create new instance
client.username_pw_set(user, password=password)  # set username and password
client.on_connect = on_connect  # attach function to callback
client.on_message = on_message  # attach function to callback

client.connect(broker_address, port=port)  # connect to broker

client.loop_start()  # start the loop

while Connected is not True:  # Wait for connection
    time.sleep(0.1)

client.subscribe("bella")

try:
    while True:
        time.sleep(1)
except (KeyboardInterrupt, SystemExit):
    print()
    print("Exiting..")
    client.disconnect()
    client.loop_stop()
