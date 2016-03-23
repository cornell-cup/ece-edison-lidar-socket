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
#include <unistd.h>

#include "mraa.h"

#define TCP_IP "192.168.4.9"
#define TCP_PORT 12345

uint8_t buf[20];
volatile int fid;
volatile int client;

struct sockaddr_in addr; // http://man7.org/linux/man-pages/man7/ip.7.html

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
					return 1;
				}
			}
		}
	}
	return 0;
}

int setupconnection(void){
	// http://gnosis.cx/publish/programming/sockets.html
	client = socket(AF_INET, SOCK_STREAM, 0);
	if (client < 0){
		printf("Failed to create socket!\n");
		return -1;
	}
	if (connect(client, (struct sockaddr*) &addr, sizeof(addr)) <0){
		fprintf(stderr, "Failed to connect to socket!\n");
		return -1;
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
	uint8_t send_buffer[724];
	
	memset(&addr, 0, sizeof(addr)); // zero the struct https://en.wikibooks.org/wiki/C_Programming/Networking_in_UNIX
	addr.sin_family = AF_INET;
	addr.sin_port = htons(TCP_PORT);
	addr.sin_addr.s_addr = inet_addr(TCP_IP); // https://en.wikibooks.org/wiki/C_Programming/Networking_in_UNIX
	
	send_buffer[0] = 0xFA; send_buffer[1] = 0xFA; send_buffer[2] = 0xFA; send_buffer[3] = 0xFA;
	uint16_t i=0;
	uint16_t tc=0;
	while (1){	
		if (getHeader()){ // detected 4 consecutive 0xFA's --> start sequence
			for (i=0; i<360; i++){
				read(fid, buf, 6);
				lidar.distance[i] = (buf[5] << 8) | buf[4];
				lidar.speed[i] = (buf[3] << 8) | buf[2];
				lidar.signal_strength[i] = (buf[1] << 8) | buf[0];
			}
			for (i=0; i<360; i++){
				tc = i*2+4;
				//printf("angle, distance = %u, %u\n", i, lidar.distance[i]);
				send_buffer[tc] = lidar.distance[i] >> 8; // high byte
				send_buffer[tc+1] = lidar.distance[i] & 0xFF; // low byte
			}
			
			if (setupconnection() < 0){
				return -1;
			}
			send(client, send_buffer, 724, 0);
		}
	}
	
	return 0;
}

