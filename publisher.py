"""Publisher code to send data using PahoMQTT."""
import paho.mqtt.client as mqtt

broker_address = "192.168.0.28"  # Broker address
port = 1883  # Broker port
user = "astr1x"  # Connection username
password = "astr1x2096"  # Connection password

client = mqtt.Client()
client.username_pw_set(user, password=password)  # set username and password
client.connect(broker_address, port)  # connect to broker

while True:
    try:
        message = input("Enter the message: ")
        client.publish("bella", message)
    except (KeyboardInterrupt, SystemExit):
        break
client.disconnect()
print()
print("Exiting..")
