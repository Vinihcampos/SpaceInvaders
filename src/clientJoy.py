import socket
'''
import Adafruit_BBIO.ADC as ADC
from time import sleep

def readAnalog(analogPin):
	ADC.setup()
	pinVal = ADC.read(analogPin)
	pinVal = pinVal * 1.8
	return pinVal
'''
HOST = '10.7.120.17'
PORT = 4325
tcp = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
dest = (HOST, PORT)
tcp.connect(dest)
msg = raw_input()
while msg <> '\x18':
	tcp.send(msg)
	msg = raw_input()
tcp.close()
	
