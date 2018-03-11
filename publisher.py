import paho.mqtt.client as mqtt

# This is the Publisher

client = mqtt.Client()
while True:
    client.connect("iot.eclipse.org", 1883, 60)
    message = input("Enter the message: ")
    client.publish("bella", message)
    client.disconnect()
