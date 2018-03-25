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
 *  P.S. All commands are followed by ':' character to mark Command Termination
 *
 *  Make sure to set the board as "Arduino/Genuino Mega 2560" before compiling.
 *
"""
import RPi.GPIO as GPIO

# One time setup at start
GPIO.setmode(GPIO.BCM)
GPIO.setup()

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

# To state the end of the command
delimiter = ':'
