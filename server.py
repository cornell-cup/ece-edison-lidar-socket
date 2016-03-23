import socket

TCP_IP = ''
print 'Started. Hope everything works!'

s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
s.bind((TCP_IP, 12345))
s.listen(1)


while True:
    c, sockaddr = s.accept()
    print 'Got connection from ', sockaddr
    data = s.recv(1)