import numpy as np
import matplotlib.pyplot as plt # http://matplotlib.org/examples/pylab_examples/polar_demo.html
import socket
from time import sleep

TCP_IP = ''
print 'Started. Hope everything works!'


#index = range(0,360)
#lidar_data = np.ones(360, dtype=np.int) # http://docs.scipy.org/doc/numpy-1.10.0/reference/generated/numpy.ones.html
#ax = plt.subplot(111, projection='polar')
#ax.plot(index, lidar_data, color='r', linewidth=3)
#plt.show()

s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
s.bind((TCP_IP, 12345))
s.listen(1)

def getData():
	global data
	c, sockaddr = s.accept()
	data = c.recv(1024)

index = range(0, 360)
lidar_data = range(0, 360)

# http://stackoverflow.com/a/15720891
plt.ion()
plt.show()

while True:
    getData()
    if not data:
        continue
    leng = len(data)
    print(str(leng))
    if leng == 724:
        #print('[0-3]: %d,%d,%d,%d' % (ord(data[0]),ord(data[1]),ord(data[2]),ord(data[3])))
        if ord(data[0])==250 and ord(data[1])==250 and ord(data[2])==250 and ord(data[3])==250:
            for i in range(0, 360):
                lidar_data[i] = (ord(data[i*2+4]) << 8) | ord(data[i*2+4+1])
                #print('i=%d : %d' % (i,lidar_data[i]))
            
            #plt.cla()
            ax = plt.subplot(111, projection='polar')
            ax.plot(index, lidar_data, color='r', linestyle='None', linewidth=1)
            plt.draw()
            print('Done')
            while True:
                a = 5
