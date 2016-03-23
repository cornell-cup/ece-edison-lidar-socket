import socket
import sys

TCP_IP = ''
print 'Started. Hope everything works!'

s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
s.bind((TCP_IP, 12345))
s.listen(1)

def getData():
	global data
	c, sockaddr = s.accept()
	data = c.recv(1024)

while True:
	getData()
	if not data:
		continue
	leng = len(data)
	print str(leng)
	