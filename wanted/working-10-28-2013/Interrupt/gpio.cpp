#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include "SimpleGPIO.h"
#include <cstring>

#define POLLTIMEOUT 10000

using namespace std;
//https://developer.ridgerun.com/wiki/index.php/Gpio-int-test.c


unsigned int LEDGPIO = 60;   // GPIO1_28 = (1x32) + 28 = 60
unsigned int TriggerGPIO = 117; // GPIO3_21 = (3x32) + 21 = 117

int main(int argc, char *argv[]){

	cout << "Testing the GPIO Pins" << endl;
	struct pollfd fdset[1];
        int nfds = 1;
        int gpio_fd, timeout, rc;
        char *buf[MAX_BUF];
        int len;
        
	gpio_export(LEDGPIO);    // The LED
	gpio_export(TriggerGPIO);  
        gpio_set_dir(LEDGPIO, OUTPUT_PIN);   // The LED is an output
	gpio_set_dir(TriggerGPIO, INPUT_PIN);
	gpio_set_edge(TriggerGPIO, "falling");
	gpio_fd = gpio_fd_open(TriggerGPIO);

	timeout = POLLTIMEOUT;

	// Flash the LED 5 times
	for(int i=0; i<5; i++)
	{
		cout << "Setting the LED on" << endl;
                gpio_set_value(LEDGPIO, HIGH);
		usleep(200000);         // on for 200ms
		cout << "Setting the LED off" << endl;
                gpio_set_value(LEDGPIO, LOW);
		usleep(200000);         // off for 200ms
	}

	while(1)
	{
		memset((void*)fdset, 0, sizeof(fdset));

		fdset[0].fd = gpio_fd; //STDIN_FILENO;
		fdset[0].events = POLLIN;

		rc = poll(fdset, nfds, timeout);
		if (rc < 0) {
			printf("\npoll() failed!\n");
			return -1;
		}
      
		if (rc == 0) {
			printf(".");
		}

		if (fdset[0].revents & POLLIN) {
			(void)read(fdset[0].fd, buf, 1);
			printf("\npoll() stdin read 0x%2.2X\n", (unsigned int) buf[0]);
		}

		fflush(stdout);		
	}

	gpio_fd_close(gpio_fd);
	gpio_unexport(LEDGPIO);     // unexport the LED
	gpio_unexport(TriggerGPIO);  // unexport the push button
	return 0;
}
