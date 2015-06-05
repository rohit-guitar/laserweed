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

unsigned int TriggerGPIO = 71; // GPIO3_21 = (3x32) + 21 = 117

int main(int argc, char *argv[]){

	cout << "Testing the GPIO edge interrupt" << endl;
	struct pollfd fdset[1];
        int nfds = 1;
        int gpio_fd, timeout, rc;
        char *buf[MAX_BUF];
        
	gpio_export(TriggerGPIO);  
	gpio_set_dir(TriggerGPIO, INPUT_PIN);
	gpio_set_edge(TriggerGPIO, "both");
	gpio_fd = gpio_fd_open(TriggerGPIO);
	timeout = POLLTIMEOUT;

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
		//getchar();
		//fflush(stdout);		
	}

	gpio_fd_close(gpio_fd);
	gpio_unexport(TriggerGPIO);  // unexport the push button
	return 0;
}
