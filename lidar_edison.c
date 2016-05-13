#include <stdio.h>
#include <stdlib.h>
#include <mraa.h>

struct lidar_data{
	uint16_t distance [360];
	uint16_t signal_strength [360];
	uint16_t speed [360];
};

int main()
{
	/* your configuration code starts here */
	const char* path = "/dev/ttyMFD1";
	mraa_uart_context uartRead = mraa_uart_init_raw(path);
	/*const char* path2 = "/dev/ttyGS0";
	mraa_uart_context uartWrite = mraa_uart_init_raw(path2);*/

	struct lidar_data lidar;

	char buf[10];
	uint16_t index;
	uint16_t speed;
	uint16_t distance;
	uint16_t signalstrength;

	for (;;){
		mraa_uart_read(uartRead, buf, 1);
		if (buf[0] == 0xFA){
			mraa_uart_read(uartRead, buf, 1);
			if ((buf[0] >= 0xA0) && (buf[0] <= 0xF9 )){
				// detected start of packet and got index
				index = (buf[0] - 0xA0) * 4;

				mraa_uart_read(uartRead, buf, 2);
				speed = (buf[1] << 8) | buf[0];
				lidar.speed[index] = speed;
				lidar.speed[index+1] = speed;
				lidar.speed[index+2] = speed;
				lidar.speed[index+3] = speed;

				mraa_uart_read(uartRead, buf, 4);
				distance = ((buf[1] & 0x3F) << 8) | buf[0];
				signalstrength = (buf[3] << 8) | buf[2];
				lidar.distance[index] = distance;
				lidar.signal_strength[index] = signalstrength;

				mraa_uart_read(uartRead, buf, 4);
				distance = ((buf[1] & 0x3F) << 8) | buf[0];
				signalstrength = (buf[3] << 8) | buf[2];
				lidar.distance[index+1] = distance;
				lidar.signal_strength[index+1] = signalstrength;

				mraa_uart_read(uartRead, buf, 4);
				distance = ((buf[1] & 0x3F) << 8) | buf[0];
				signalstrength = (buf[3] << 8) | buf[2];
				lidar.distance[index+2] = distance;
				lidar.signal_strength[index+2] = signalstrength;

				mraa_uart_read(uartRead, buf, 4);
				distance = ((buf[1] & 0x3F) << 8) | buf[0];
				signalstrength = (buf[3] << 8) | buf[2];
				lidar.distance[index+3] = distance;
				lidar.signal_strength[index+3] = signalstrength;
			}
		}
	}
	return 0;
}
