#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include "SimpleGPIO.h"

/****************************************************************
 * Main
 ****************************************************************/
int main(int argc, char **argv, char **envp)
{
	struct pollfd fdset[1];
	int nfds = 1;
	int gpio_fd, timeout, rc;
	char *buf[MAX_BUF];
	unsigned int gpio;
	int len, intr_count;
	double start, end; 
	int flag; 

	if (argc < 2) {
		printf("Usage: gpio-int <gpio-pin>\n\n");
		printf("Waits for a change in the GPIO pin voltage level or input on stdin\n");
		exit(-1);
	}

	gpio = atoi(argv[1]);
	gpio_export(gpio);
	gpio_set_dir(gpio, INPUT_PIN);
	gpio_set_edge(gpio, "rising");
	gpio_fd = gpio_fd_open(gpio);
	int i=0; 
	timeout = POLL_TIMEOUT;
	while (1) {
		memset((void*)fdset, 0, sizeof(fdset));
		fdset[0].fd = gpio_fd;
		fdset[0].events = POLLPRI;
		rc = poll(fdset, nfds, timeout);
		if (rc < 0) {
			printf("\npoll() failed!\n");
			return -1;
		}
		if (rc == 0) {
			printf(".");
		}
		if (fdset[0].revents & POLLPRI) 
		{	
			(void)read(fdset[0].fd, buf, 1);
                        i++;
                        if(i<=81)
                        continue;
                        printf("\npoll() stdin read\n");
			i=0;  
		}
		fflush(stdout);
	}
	gpio_fd_close(gpio_fd);
	return 0;
}

