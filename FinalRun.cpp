/*
 * LaserWeed.cpp
 * Author: sudhir vyasaraja
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
#define DEVICE_PORT "/dev/ttyO2"
#endif
using namespace cv;
unsigned int LaserGPIO[16];
std::vector<string> tokens, tokens_timeout;

/*************************************Main function*****************************************************************************************************/
int main(int argc, char* argv[])
{
/************************************************************************Variables**********************************************************************/
static int LaserState[32][16];		//Create 32x16 matrix to hold the laser state values.
static unsigned int Laser1[8], Laser2[8], BufferLaser[8]; // laser1[8] is the first row laser similarly laser2[18], while buffer laser[8] is to store the values of laser fired to log the data.
char File_Name_For_Image[10000];
CvCapture* capture = NULL;
IplImage* img = NULL;
int image_num = 0;
int Did_I_Reach_The_First_Row=0;
int NoOfPulses=0, intr_count=0;
int Count_For_Rows = 0;
std::string Laser1str, Laser2str;
std::ostringstream convert;
std::ofstream outputfile;

/*serialib LS;                                                            
int Ret;                                                                
char Buffer[128], temp_Buffer[128];
Ret=LS.Open(DEVICE_PORT,9600);                                        
if (Ret!=1) 
{                                                           
printf ("Error while opening port. Permission problem ?\n");        
return Ret;                                                         
}
while(1)
{
LS.ReadString(Buffer,'\n',1280,5000);
printf(Buffer);
}*/
/************************************************Interrupt part**************************************************************************************/
struct pollfd fdset[1];
int nfds = 1;
int gpio_fd, timeout, rc;
char *buf[MAX_BUF];
unsigned int gpio;
gpio = 20;
gpio_export(gpio);
gpio_set_dir(gpio, INPUT_PIN);
gpio_set_edge(gpio, "rising");
gpio_fd = gpio_fd_open(gpio);
timeout = POLL_TIMEOUT;
/************************************************turning the motor on**************************************************************************************/
unsigned int gpio_for_motor;
gpio_for_motor= 61;
gpio_export(gpio_for_motor);
gpio_set_dir(gpio_for_motor, OUTPUT_PIN);
gpio_set_value(gpio_for_motor, LOW);
/******************Initializing laser GPIO's********************************************************************************************************/
LaserGPIO[0] = 66;		// P8_07
LaserGPIO[1] = 67;		// P8_08
LaserGPIO[2] = 69;		// P8_09
LaserGPIO[3] = 68;		// P8_10
LaserGPIO[4] = 45;		// P8_11
LaserGPIO[5] = 44;		// P8_12
LaserGPIO[6] = 30;		// P9_11
LaserGPIO[7] = 26;		// P8_14
LaserGPIO[8] = 47;		// P8_15
LaserGPIO[9] = 46;		// P8_16
LaserGPIO[10] = 27;		// P8_17
LaserGPIO[11] = 65;		// P8_18
LaserGPIO[12] = 60;		// P9_12
LaserGPIO[13] = 31;		// P9_13
LaserGPIO[14] = 50;		// P9_14
LaserGPIO[15] = 48;		// p9_15

for(int No_Of_Laser=0; No_Of_Laser<16; No_Of_Laser++)
{
gpio_export(LaserGPIO[No_Of_Laser]);
gpio_set_dir(LaserGPIO[No_Of_Laser], OUTPUT_PIN);
}
/**********************************************Initializing_camera*********************************************************************************/
capture = NULL;
img = NULL;
capture = cvCaptureFromCAM(-1);
cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH, 160 );                  // 16 lasers in one column
cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT, 120 );                 // to get a square image

if ( !capture )
{
fprintf( stderr, "ERROR: capture is NULL \n" );
getchar();
return -1;
}
img = cvQueryFrame( capture );
if ( !img )
{
fprintf( stderr, "ERROR: img is null...\n" );
getchar();
return -1;
}
sprintf(File_Name_For_Image, "/home/root/Desktop/laserweed/photos/%u.jpg", image_num);
if(!cvSaveImage(File_Name_For_Image,img))
{
std::cout << "Save image failed! Check code!" << std::endl;
}
IplImage* imgGreenThresh = GetThresholdedImage(img);					//Perform thresholding operation to get the binary image.
sprintf(File_Name_For_Image, "/home/root/Desktop/laserweed/thresholdphotos/%u.jpg", image_num);
if(!cvSaveImage(File_Name_For_Image,imgGreenThresh))
{
std::cout << "Save image failed! Check code!" << std::endl;
}
Count_For_Rows = Count_For_Rows + 5;
GetImageMatrix(imgGreenThresh, LaserState, BLOCKSIZE_Height, BLOCKSIZE_Width, OCCUPANCYPERCENT, Count_For_Rows);//Get the LaserState Matrix generated from the thresholded image
cvReleaseCapture (&capture);
cvReleaseImage (&img);

/**** Laser data copied to a string to print to output file *********************************************************************************/
Laser1str = "";
Laser2str = "";
for(int No_Of_Laser=0; No_Of_Laser<8; No_Of_Laser++)
{
convert.str("");
convert.clear();
convert << Laser1[No_Of_Laser];
Laser1str = Laser1str + convert.str();
convert.str("");
convert.clear();
convert << Laser2[No_Of_Laser];
Laser2str = Laser2str + convert.str();
}

bool check =true;

/*while(1)
{
while(check)
{
memset((void*)fdset, 0, sizeof(fdset));
fdset[0].fd = gpio_fd;
fdset[0].events = POLLPRI;
rc = poll(fdset, nfds, timeout);
if (rc < 0) 
{
printf("\npoll() failed!\n");
return -1;
}
if (rc == 0) 
{
printf(".");
}
if (fdset[0].revents & POLLPRI)
{
unsigned int las_1;
las_1=66;
gpio_set_value(las_1, HIGH);
//gpio_set_value(gpio_for_motor, HIGH);
(void)read(fdset[0].fd, buf, 1);
intr_count++;
std::cout<<intr_count<<std::endl;
NoOfPulses++;
if(NoOfPulses ==57){
	gpio_set_value(gpio_for_motor, LOW);
	check=false;break;
}
}
if(Did_I_Reach_The_First_Row == 1)
{
if(intr_count >= 150)
{
ChangeLaserStates(LaserState, BufferLaser, Laser1, Laser2, Count_For_Rows);
Count_For_Rows = Count_For_Rows - 1;

for(int i=0; i<8; i++)
{
if(Laser1[i] == 0)
gpio_set_value(LaserGPIO[i], LOW);
else
gpio_set_value(LaserGPIO[i], HIGH);
if(Laser2[i] == 0)
gpio_set_value(LaserGPIO[i+8], LOW);
else
gpio_set_value(LaserGPIO[i+8], HIGH);
}
for(int i=0; i<16; i++)
{
for(int j=0; j<16; j++)
{
printf("%d ", LaserState[i][j]);
}
printf("\n");
}
printf("\n\n");

Laser1str = "";
Laser2str = "";
for(int i=0; i<8; i++)
{
convert.str("");
convert.clear();
convert << Laser1[i];
Laser1str = Laser1str + convert.str();
convert.str("");
convert.clear();
convert << Laser2[i];
Laser2str = Laser2str + convert.str();
}
intr_count = 0;
}
}
else
{
if(intr_count >= 1575)
{
ChangeLaserStates(LaserState, BufferLaser, Laser1, Laser2, Count_For_Rows);
Count_For_Rows = Count_For_Rows - 1;
std::cout << Count_For_Rows << std::endl;
for(int i=0; i<8; i++)
{
if(Laser1[i] == 0)
gpio_set_value(LaserGPIO[i], LOW);
else
gpio_set_value(LaserGPIO[i], HIGH);
if(Laser2[i] == 0)
gpio_set_value(LaserGPIO[i+8], LOW);
else
gpio_set_value(LaserGPIO[i+8], HIGH);
}
for(int i=0; i<16; i++)
{
for(int j=0; j<16; j++)
{
printf("%d ", LaserState[i][j]);
}
printf("\n");
}
printf("\n\n");

Laser1str = "";
Laser2str = "";
for(int i=0; i<8; i++)
{
convert.str("");
convert.clear();
convert << Laser1[i];
Laser1str = Laser1str + convert.str();
convert.str("");
convert.clear();
convert << Laser2[i];
Laser2str = Laser2str + convert.str();
}
intr_count = 0;
Did_I_Reach_The_First_Row = 1;

}
}
if(NoOfPulses >= 1050)
{
capture = NULL;
img = NULL;
capture = cvCaptureFromCAM(-1);
cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH, 160 );                  // 16 lasers in one column
cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT, 120 );                 // to get a square image
cvSetCaptureProperty(capture, CV_CAP_PROP_FPS, 1);
if ( !capture )
{
fprintf( stderr, "ERROR: capture is NULL \n" );
getchar();
return -1;
}
img = cvQueryFrame( capture );
if ( !img )
{
fprintf( stderr, "ERROR: img is null...\n" );
getchar();
return -1;
}
image_num++;
sprintf(File_Name_For_Image, "/home/root/Desktop/laserweed/photos/%u.jpg", image_num);
if(!cvSaveImage(File_Name_For_Image,img))
{
std::cout << "Save image failed!" << std::endl;
break;
}
IplImage* imgGreenThresh = GetThresholdedImage(img);                                    //Perform thresholding operation to get the binary image.
sprintf(File_Name_For_Image, "/home/root/Desktop/laserweed/thresholdphotos/%u.jpg", image_num);
if(!cvSaveImage(File_Name_For_Image,imgGreenThresh))
{
std::cout << "Save image failed! Check code!" << std::endl;
}
Count_For_Rows = Count_For_Rows + 6;
GetImageMatrix(imgGreenThresh, LaserState, BLOCKSIZE_Height, BLOCKSIZE_Width, OCCUPANCYPERCENT, Count_For_Rows);//Get the LaserState Matrix generated from the thresholded image
cvReleaseCapture (&capture);
cvReleaseImage (&img);

std::cout << Count_For_Rows << std::endl;
NoOfPulses = 0;
}
}
}
for(int i=0; i<16; i++)
{
gpio_unexport(LaserGPIO[i]);
}
gpio_fd_close(gpio_fd);*/
return 0;
}
