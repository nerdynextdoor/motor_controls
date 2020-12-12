#!/usr/bin/env python
import socket
import serial
import os 
import time

ser = serial.Serial('/dev/ttyS0', 9600, timeout=1)
TCP_IP = '192.168.0.149'
TCP_PORT = 8989
BUFFER_SIZE = 20  # Normally 1024, but we want fast response

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((TCP_IP, TCP_PORT))
s.listen(1)

conn, addr = s.accept()
print 'Connected'
print 'Connection address:', addr
s.sendall(b'Connect')
while 1:
	try:
	    data = conn.recv(BUFFER_SIZE)
	    if not data: break
	    print "received data:", data
	    if(data == 'q'):
	    	print "Closing program"
	    	os.system("sudo shutdown -h now")
	    	time.sleep(0.05)
	    	break
	    else:
		    string = data.encode()
	        ser.write(string)
    except KeyboardInterrupt:
    	print "Terminating server"
        break
conn.close()