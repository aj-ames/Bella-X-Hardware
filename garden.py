"""
This file has all the garden functions
"""
import RPi.GPIO as GPIO


# Pins for motor
motor1Pin1 = 17  # Physical pin: 11
motor1Pin2 = 18  # Physical pin: 12
motor1En = 27  # Physical pin: 13
sprinkler_on = False


def garden_status():
    moist = moistureSampler()
    moistureAvg = 100 - ((moist / 1024.0) * 100.0)
    if moistureAvg < 10:
        print_string = "M10" + str(moistureAvg) + ":"
        send(print_string)
    else:
        print_string = "M1" + str(moistureAvg) + ":"
        send(print_string)


def garden_sprinkler():
    moist = moistureSampler()
    moistureAvg = moistureAvg = 100 - ((moist / 1024.0) * 100.0)
    if moistureAvg > 90:
        # Water is too wet
        send("F3:")
    else:
        startSprinkler()
        send("T3:")


def startSprinkler():
    global sprinkler_on
    GPIO.output(motor1En, True)
    GPIO.output(motor1Pin1, True)
    GPIO.output(motor1Pin2, False)
    sprinkler_on = True


def stopSprinkler():
    global sprinkler_on
    GPIO.output(motor1En, False)
    GPIO.output(motor1Pin1, False)
    GPIO.output(motor1Pin2, False)
    sprinkler_on = False


def moistureSampler():
    sum = 0
    for _ in range(25):
        sum += 0  # get_analog_value(moisturePin)
    return sum


def send():
    # Some MQTT thing
    pass
