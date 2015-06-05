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
#include <sys/time.h> 

#define POLLTIMEOUT 1000

using namespace std;
//https://developer.ridgerun.com/wiki/index.php/Gpio-int-test.c


unsigned int LEDGPIO = 60;   // GPIO1_28 = (1x32) + 28 = 60
unsigned int TriggerGPIO = 48; // GPIO1_16 = (1x32) + 16 = 48

double get_wall_time(){
    struct timeval time;
    if (gettimeofday(&time,NULL)){
        //  Handle error
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

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
	gpio_set_edge(TriggerGPIO, "rising");
	if(!gpio_fd_open(TriggerGPIO))
		printf("Open failed!\n");
	else
		gpio_fd = gpio_fd_open(TriggerGPIO);
	timeout = POLLTIMEOUT;
	int i=0; 
	double start = get_wall_time();  
	while(1)
	{
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
		if (fdset[0].revents & POLLPRI) {
				len = read(fdset[0].fd, buf, MAX_BUF);
				printf("\npoll() GPIO %d interrupt occurred\n", TriggerGPIO);
		}
		fflush(stdout);
	}
	double end = get_wall_time(); 
	cout << i << " interrupts in " << end-start << " seconds" << endl; 

	gpio_fd_close(gpio_fd);
	gpio_unexport(LEDGPIO);    
	gpio_unexport(TriggerGPIO); 
	return 0;
}
