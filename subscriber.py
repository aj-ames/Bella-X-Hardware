"""Subscriber code to receive data using PahoMQTT."""
import paho.mqtt.client as mqttClient
import time
import argparse
import sys


def on_connect(client, userdata, flags, rc):
    """on_connect is used to check connection to broker."""
    if rc == 0:

        print("Connected to broker: " + str(broker_address))
        global Connected  # Use global variable
        Connected = True  # Signal connection

    else:
        print("Connection failed to " + str(broker_address))


def on_message(client, userdata, message):
    """on_message is executed whenever a message is received."""
    print("Message received: " + str(message.payload.decode("utf-8")),
          ", Topic: ", message.topic, ", Retained: ", bool(message.retain))


def get_args_values(args=None):
    """Method to handle command line arguments."""
    parser = argparse.ArgumentParser(description="Arguments supported..")
    parser.add_argument('-H', '--host',
                        help='Broker IP',
                        default='localhost')
    parser.add_argument('-p', '--port',
                        help='port of the Broker',
                        default='1883')
    parser.add_argument('-u', '--user',
                        help='user name',
                        default='astr1x')
    parser.add_argument('-P', '--password',
                        help="password",
                        default='astr1x2096')

    info = parser.parse_args(args)
    return (info.host,
            info.port,
            info.user,
            info.password)


"""
Get Broker address, port, username and password.
The default value has already been provided.
"""
if __name__ == '__main__':
    broker_address, port, user, password = get_args_values(sys.argv[1:])
    port = int(port)


Connected = False  # global variable for the state of the connection

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
