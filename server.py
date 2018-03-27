"""
 * Code to run on the Server Arduino Board.
 *  This code will redirect the command code issued by Bella to the appropriate
 * child node.
 *
 *  Commands:
 *  RL1O - Room Light 1 On
 *  RL1F - Room Light 2 On
 *  RL2O - Room Light 1 On
 *  RL2F - Room Light 2 On
 *  RLS - Room Lights Status
 *  KS - Kitchen Status
 *  GSS - Garden status; return the moisture
 *  GSO - Garden sprinkler turned ON
 *  GSF - Garden sprinkler turned OFF
 *  FT - Fan turned On
 *  FF - Fan turned Off
 *
 *
 *  PS: All commands are followed by ':' character to mark Command Termination
 *
"""
import RPi.GPIO as GPIO

# One time setup at start
GPIO.setmode(GPIO.BCM)
GPIO.setup()
room_cmd_list = ["RL1O", "RL1F", "RL2O", "RL2F", "RLS", "FT", "FF"]
kitchen_cmd_list = ["KS"]
garden_cmd_list = ["GSS", "GSO", "GSF"]

# Pin 2 and 3 for Relay
light1 = 2  # Physical pin: 3
light2 = 3  # Physical pin: 5

# Pins for Ultrasonic Sesnors for Kitchen
tp1 = 25  # Physical pin: 22
ep1 = 8  # Physical pin: 24
tp2 = 7  # Physical pin: 26
ep2 = 1  # Physical pin: 28

# Initial values for the automation in garden
wateringTime = 40
triggerMiostureContent = 200
warningMoistureContent = 750

# Pins for motor
motor1Pin1 = 17  # Physical pin: 11
motor1Pin2 = 18  # Physical pin: 12
motor1En = 27  # Physical pin: 13
motor2pin1 = 22  # Physical pin: 15
motor2pin2 = 23  # Physical pin: 16
motor2En = 24  # Physical pin: 18

# To state the end of the command
delimiter = ':'
