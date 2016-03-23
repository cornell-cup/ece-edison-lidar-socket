/* Build on top of mculidar to set up a network socket and send data over
 *
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <fcntl.h>
#include <string.h>

#include "mraa.h"

#define TCP_IP "192.168.4.9"
#define TCP_PORT 12345

uint8_t buf[20];
volatile int fid;
volatile int client;

struct lidar_data{
	uint16_t distance [360];
	uint16_t signal_strength [360];
	uint16_t speed [360];
};

int getHeader(void){
	read(fid, buf, 1);
	if (buf[0] == 0xFA){
		read(fid, buf, 1);
		if (buf[0] == 0xFA){
			read(fid, buf, 1);
			if (buf[0] == 0xFA){
				read(fid, buf, 1);
				if (buf[0] == 0xFA){
					buf[0] = 0xFA;
					buf[1] = 0xFA;
					buf[2] = 0xFA;
					buf[3] = 0xFA;
					send(client, buf, 4, 0);
					return 1;
				}
			}
		}
	}
	return 0;
}

int main(){
	fid = open("/dev/ttymcu0", O_RDONLY);
	if (fid == 0){
		fprintf(stderr,"Couldn't open communication with MCU!\n");
		return -1;
	}
	printf("Opened communication with MCU!\n");
	
	struct lidar_data lidar;
	
	// http://gnosis.cx/publish/programming/sockets.html
	client = socket(AF_INET, SOCK_STREAM, 0);
	if (client < 0){
		fprintf(stderr, "Failed to create socket!\n");
		return -1;
	}
	struct sockaddr_in addr; // http://man7.org/linux/man-pages/man7/ip.7.html
	memset(&addr, 0, sizeof(addr));                /* zero the struct */ // https://en.wikibooks.org/wiki/C_Programming/Networking_in_UNIX
	addr.sin_family = AF_INET;
	addr.sin_port = htons(TCP_PORT);
	addr.sin_addr.s_addr = inet_addr(TCP_IP); // https://en.wikibooks.org/wiki/C_Programming/Networking_in_UNIX
	if (connect(client, (struct sockaddr*) &addr, sizeof(addr)) <0){
		fprintf(stderr, "Failed to connect to socket!\n");
		return -1;
	}
	printf("Connected to socket!\n");
	
	
	uint16_t i=0;
	uint8_t tc=0;
	while (1){
		if (getHeader()){ // detected 4 consecutive 0xFA's --> start sequence
			for (i=0; i<360; i++){
				read(fid, buf, 6);
				lidar.distance[i] = (buf[5] << 8) | buf[4];
				lidar.speed[i] = (buf[3] << 8) | buf[2];
				lidar.signal_strength[i] = (buf[1] << 8) | buf[0];
				send(client, buf, 6, 0);
			}
			for (i=0; i<360; i++){
				printf("angle, distance = %u, %u\n", i, lidar.distance[i]);
			}
		}
	}
	
	return 0;
}
