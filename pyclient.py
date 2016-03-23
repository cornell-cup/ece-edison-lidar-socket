import socket   #for sockets
import sys  #for exit

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
 
print 'Socket Created'
 
remote_ip = '192.168.4.9'
port = 12345

#Connect to remote server
s.connect((remote_ip , port))
 
#Send some data to remote server
message = "SEND DATA YAY!"
 
try :
    #Set the whole string
    s.sendall(message)
except socket.error:
    #Send failed
    print 'Send failed'
    sys.exit()
 
print 'Message send successfully'