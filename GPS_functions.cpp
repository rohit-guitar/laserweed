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
#include <poll.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <cxcore.h>

#include "LaserWeed.h"
#include "SimpleGPIO.h"
#include "serialib.h"

#define BLOCKSIZE_Width 10	
#define BLOCKSIZE_Height 20					//N in a NxN pixel block for processing-depends on image resolution
#define OCCUPANCYPERCENT 5					//Percent of cell covered by vegetation
#ifdef __linux__
#define DEVICE_PORT "/dev/ttyO4"
#endif
