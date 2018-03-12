"""Publisher code to send data using PahoMQTT."""
import paho.mqtt.client as mqtt
import argparse
import sys


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
                        default="astr1x2096")

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

client = mqtt.Client()
client.username_pw_set(user, password=password)  # set username and password
client.connect(broker_address, port)  # connect to broker

while True:
    try:
        message = input("Enter the message: ")
        client.publish("bella", message, qos=1, retain=True)
    except (KeyboardInterrupt, SystemExit):
        break
client.disconnect()
print()
print("Exiting..")
