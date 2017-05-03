import socket
import Adafruit_BBIO.GPIO as GPIO
import Adafruit_BBIO.ADC as ADC
from time import sleep

#Button setup
GPIO.setup("P9_30", GPIO.IN)

def readAnalog(analogPin):
    ADC.setup()
    pinVal = ADC.read(analogPin)
    pinVal = pinVal * 4096
    return int(pinVal)

def ldr():
    ldr = readAnalog("P9_38")
    return ldr <= 35

def potenciometer():
    pot = readAnalog("P9_40")
    sleep(0.01)
    pot2 = readAnalog("P9_40")
    dir = 's'
    if abs(pot2 - pot) > 10:
        if pot2 - pot > 10:
            dir = 'r'
        elif pot2 - pot < -10:
            dir = 'l'
    if abs(pot - 4096) <= 3:
        dir = 'r'
    elif pot <= 7:
        dir = 'l'
    return dir

def button():
    return GPIO.input("P9_30")

HOST = '127.0.0.1'
PORT = 4325
tcp = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
dest = (HOST, PORT)
tcp.connect(dest)
msg = ''
while msg <> '\x18':
    move = potenciometer()
    shoot = ldr()
    bye = button()
    msg = ''
    if move != 's':
        msg = move + ' 0 0'
    else:
        if shoot:
            msg = 's 1 0'
        elif bye:
            msg = 's 0 1'
        else:
            msg = 's 0 0'
    tcp.send(msg)
tcp.close()