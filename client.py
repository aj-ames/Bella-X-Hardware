"""Paho MQTT."""
import paho.mqtt.client as mqtt

received_message = ""


def on_message(subscriber, userdata, msg):
    received_message = str(msg.payload.decode("utf-8"))
    # print received_message
    print(received_message)


client = mqtt.Client()
client.on_message = on_message
client.connect("iot.eclipse.org", 1883, 60)
client.subscribe("bella")
client.loop_forever()
