/*
 * GPS_functions.cpp
 * Author: Revant Shah
 * check the overlap of the matrix when new image is obtained
 */
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>

#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>




#include "serialib.h"
std::vector<string> tokens, tokens_timeout; 

#ifdef __linux__
#define DEVICE_PORT "/dev/ttyO2"
#endif

int main(int argc, char* argv[])
{
serialib LS;                                                            
int Ret; 
Ret=LS.Open(DEVICE_PORT,9600);                                        
	if (Ret!=1) 
	{                                                           
		printf ("Error while opening port. Permission problem ?\n");        
        return Ret;                                                         
	}
printf ("something");
}

