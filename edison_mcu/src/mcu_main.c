#include "mcu_api.h"
#include "mcu_errno.h"
#include <stdio.h>
#include <stdint.h>

struct lidar_data{
	uint16_t distance [360];
	uint16_t signal_strength [360];
	uint16_t speed [360];
};

void mcu_main()
{
	/* your configuration code starts here */
	gpio_setup(48, 1);  /* set GPIO 48 as output */
	uart_setup(1, 115200);

	struct lidar_data lidar;

	uint8_t buf[10];
	uint16_t index;
	uint16_t speed;
	uint16_t distance;
	uint16_t signalstrength;

	uint16_t i;

	while (1){
		uart_read(1, buf, 1);
		if (buf[0] == 0xFA){
			uart_read(1, buf, 1);
			if ((buf[0] >= 0xA0) && (buf[0] <= 0xF9 )){
				// detected start of packet and got index
				index = (buf[0] - 0xA0) * 4;

				uart_read(1, buf, 2);
				speed = (buf[1] << 8) | buf[0];
				lidar.speed[index] = speed;
				lidar.speed[index+1] = speed;
				lidar.speed[index+2] = speed;
				lidar.speed[index+3] = speed;

				uart_read(1, buf, 4);
				distance = ((buf[1] & 0x3F) << 8) | buf[0];
				signalstrength = (buf[3] << 8) | buf[2];
				lidar.distance[index] = distance;
				lidar.signal_strength[index] = signalstrength;

				uart_read(1, buf, 4);
				distance = ((buf[1] & 0x3F) << 8) | buf[0];
				signalstrength = (buf[3] << 8) | buf[2];
				lidar.distance[index+1] = distance;
				lidar.signal_strength[index+1] = signalstrength;

				uart_read(1, buf, 4);
				distance = ((buf[1] & 0x3F) << 8) | buf[0];
				signalstrength = (buf[3] << 8) | buf[2];
				lidar.distance[index+2] = distance;
				lidar.signal_strength[index+2] = signalstrength;

				uart_read(1, buf, 4);
				distance = ((buf[1] & 0x3F) << 8) | buf[0];
				signalstrength = (buf[3] << 8) | buf[2];
				lidar.distance[index+3] = distance;
				lidar.signal_strength[index+3] = signalstrength;

				if (index == 356){ // one entire set of information received
					buf[0] = 0xFA;
					buf[1] = 0xFA;
					buf[2] = 0xFA;
					buf[3] = 0xFA;
					host_send(buf, 4); // send start sequence
					for (i=0; i<360; i++){
						buf[5] = lidar.distance[i] >> 8;
						buf[4] = lidar.distance[i] & 0xFF;
						buf[3] = lidar.speed[i] >> 8;
						buf[2] = lidar.speed[i] & 0xFF;
						buf[1] = lidar.signal_strength[i] >> 8;
						buf[0] = lidar.signal_strength[i] & 0xFF;
						host_send(buf, 6);
					}
					mcu_sleep(200); // 2s
				}
			}
		}
	}

}
