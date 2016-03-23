/* Communicate with the MCU that's sending lidar data
 * Print the data
 */

#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>

#include "mraa.h"

uint8_t buf[20];
volatile int fid;

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

int main(){
	fid = open("/dev/ttymcu0", O_RDONLY);
	if (fid == 0){
		fprintf(stderr,"Couldn't open communication with MCU!\n");
		return -1;
	}
	printf("Opened communication with MCU!\n");
	
	struct lidar_data lidar;
	
	uint16_t i=0;
	uint8_t tc=0;
	while (1){
		if (getHeader()){ // detected 4 consecutive 0xFA's --> start sequence
			for (i=0; i<360; i++){
				read(fid, buf, 6);
				lidar.distance[i] = (buf[5] << 8) | buf[4];
				lidar.speed[i] = (buf[3] << 8) | buf[2];
				lidar.signal_strength[i] = (buf[1] << 8) | buf[0];
			}
			for (i=0; i<360; i++){
				printf("angle, distance = %u, %u\n", i, lidar.distance[i]);
			}
		}
	}
	
	return 0;
}

	/*
	// To test transmission over connection:
	buf[0] = 0xFA;
	buf[1] = 33;
	buf[2] = 33;
	buf[3] = 33;
	send(client, send_buffer, 724, 0);
	
	return 0;
	*/