/*
 * LaserWeed.cpp
 * Author: sudhir vyasaraja
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

#define BLOCKSIZE 11														//N in a NxN pixel block for processing-depends on image resolution
#define OCCUPANCYPERCENT 5													//Percent of cell covered by vegetation
#define NOOFPULSES 2000*(4096/(4*3.1412*4))											//No of pulses for every 0.25 inch travelled

#define P9_42 7
#define P9_41 20

#ifdef __linux__
#define DEVICE_PORT "/dev/ttyO4"
#endif

using namespace cv;
unsigned int LaserGPIO[16];
std::vector<string> tokens, tokens_timeout; 

/*************************************************************************************************************/
//Main Function
int main(int argc, char* argv[])
{
	int i, j, count, NoOfPulses=0;
	static int LaserState[16][16];												//Create 16x16 matrix to hold the laser state values.
	static unsigned int Laser1[8], Laser2[8], BufferLaser[8]; 						
	char Filename[10000];
	//sudhir 12/11/2013
	//char c; 
	CvCapture* capture = NULL;
	IplImage* img = NULL;
	int image_num = 0;      

	serialib LS;                                                            
   	int Ret;                                                                
	char Buffer[128], temp_Buffer[128];
	Ret=LS.Open(DEVICE_PORT,19200);                                        
	if (Ret!=1) 
	{                                                           
		printf ("Error while opening port. Permission problem ?\n");        
        return Ret;                                                         
	}
	
	std::string Laser1str, Laser2str; 
	std::ostringstream convert; 
	
	std::ofstream outputfile; 
	outputfile.open("GPS_Laser_Log.txt");
	if(!outputfile.is_open())
	{
		printf("Error opening the output file\n. Data cannot be logged!\n");
		return 1;
	}
	
	/******************************************************/
	/* Interrupt part*/
	
	struct pollfd fdset[1];
	int nfds = 1;
	int gpio_fd, timeout, rc;
	char *buf[MAX_BUF];
	unsigned int gpio;
	// sudhir 12/11/2013
	//int len;
	//sudhir 12/11/2013
	// if (argc < 2) {
		// printf("Usage: gpio-int <gpio-pin>\n\n");
		// printf("Waits for a change in the GPIO pin voltage level or input on stdin\n");
		// exit(-1);
	// }
	//sudhir 12/11/2013
	//gpio = atoi(argv[1]);
	
	//forward_pin
	gpio_export(P9_41);
	gpio_set_dir(P9_41, INPUT_PIN);
	gpio_set_edge(P9_41, "rising");
	gpio_fd_Forward = gpio_fd_open(P9_41);
	
	
	//backward pin
	gpio_export(P9_42);
	gpio_set_dir(P9_42, INPUT_PIN);
	gpio_set_edge(P9_42, "rising");
	gpio_fd_Backward = gpio_fd_open(P9_42);
	
	int intr_count=0; 
	timeout = POLL_TIMEOUT;
	/******************************************************/
	
	//LaserGPIO initiation
	LaserGPIO[0] = 66;		// P8_07
	LaserGPIO[1] = 67;		// P8_08
	LaserGPIO[2] = 69;		// P8_09
	LaserGPIO[3] = 68;		// P8_10
	LaserGPIO[4] = 45;		// P8_11
	LaserGPIO[5] = 44;		// P8_12
	LaserGPIO[6] = 23;		// P8_13
	LaserGPIO[7] = 26;		// P8_14
	LaserGPIO[8] = 47;		// P8_15
	LaserGPIO[9] = 46;		// P8_16
	LaserGPIO[10] = 27;		// P8_17
	LaserGPIO[11] = 65;		// P8_18
	LaserGPIO[12] = 22;		// P8_19
	LaserGPIO[13] = 61;		// P8_26
	LaserGPIO[14] = 30;		// P9_11
	LaserGPIO[15] = 60;		// p9_12

	for(i=0; i<16; i++)
	{
		gpio_export(LaserGPIO[i]);
		gpio_set_dir(LaserGPIO[i], OUTPUT_PIN);
	}
	
/******************************************************************************************/
/*sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss*/
/* Working code of capture process and saving it in a folder - sudhir 10/28/2013 */ 

		
                capture = NULL;
                img = NULL;
                capture = cvCaptureFromCAM(-1);
                cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH, 160 );                  // 16 lasers in one column
                cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT, 160 );                 // to get a square image
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
                sprintf(Filename, "/home/root/Desktop/Laserweed/photos/%u.jpg", image_num);
                if(!cvSaveImage(Filename,img))
		{
			std::cout << "Save image failed! Check code!" << std::endl; 
		}
		
		
		//img = cvLoadImage("/home/root/Desktop/Laserweed/1.jpg", 1);
		//img = cvLoadImage("/home/root/Desktop/Laserweed/2.jpg", 1);

		sprintf(Filename, "/home/root/Desktop/Laserweed/photos/%u.jpg", image_num);
                if(!cvSaveImage(Filename,img))
                {
                        std::cout << "Save image failed! Check code!" << std::endl;
                }
		IplImage* imgGreenThresh = GetThresholdedImage(img);					//Perform thresholding operation to get the binary image.
                sprintf(Filename, "/home/root/Desktop/Laserweed/thresholdphotos/%u.jpg", image_num);
                if(!cvSaveImage(Filename,imgGreenThresh))
                {
                        std::cout << "Save image failed! Check code!" << std::endl;
                }
		GetImageMatrix(imgGreenThresh, LaserState, BLOCKSIZE, OCCUPANCYPERCENT);		//Get the LaserState Matrix generated from the thresholded image
		cvReleaseCapture (&capture); 
		cvReleaseImage (&img); 
		
		/**** Laser data copied to a string to print to output file ****/
		Laser1str = ""; 
		Laser2str = ""; 
		for(i=0; i<8; i++)
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
		
		/****  GPS data giving the initial location of the machine ****/
		
		Ret=LS.ReadString(Buffer,'\n',128,5000);          								// timeout of 5 secs                      
		if (Ret>0)
		{       
			char *ch;
  			ch = strtok(Buffer, ",");
			tokens.clear(); 
	  		while (ch != NULL) 
			{
				tokens.push_back(ch);  
  				ch = strtok(NULL, ",");
  			}
			tokens_timeout.clear(); 
			tokens_timeout = tokens;
			outputfile << tokens[1] << " " << tokens[2] << " " << tokens[4] << " " << Laser1str << " " << Laser2str << std::endl;
			Laser1str = ""; 
			Laser2str = ""; 
		}
		else
		{
			std::cout << "GPS data could not be found. Time out!" << std::endl;
			return 1;
		}
		
		unsigned int forward_dir 0;
		unsigned int backward_dir 0;
		
		unsigned int latest_forward_read 0;
		unsigned int last_forward_read 0;
		
		
/******************************************************************************************************************/

	while(1)
	{
		memset((void*)fdset, 0, sizeof(fdset));
		fdset[0].fd = gpio_fd_Forward;
		fdset[0].events = POLLPRI;
		
		fdset[1].fd = gpio_fd_Backward;
		fdset[1].events = POLLPRI;
		
		rc = poll(fdset, nfds, timeout);      
		if (rc < 0) {
			printf("\npoll() failed!\n");
			return -1;
		}
		if (rc == 0) {
			printf(".");
		}
		
		latest_forward_read = fdset[0].revents & POLLPRI;
		
		if (!latest_forward_read && last_forward_read) if(fdset[1].revents & POLLPRI) intr_count--;
		else intr_count++;
		
		last_forward_read = latest_forward_read;
		
		
		//81 intr_counts corresponds an increment in NoOfPulses
		//81 intr_count also corresponds to 0.25in
		
		//distance between camera and laser is 2 inch
		
		
		if(intr_count >= 81 || intr_count <= 0) 
			if(!intr_count) intr_count = 81;
			else intr_count = 0;
			
			if(NoOfPulses == 8)	{
				// sudhir - 12/11/2013
				//static int img_num=0; 
				
				capture = NULL;
						img = NULL;
						capture = cvCaptureFromCAM(-1);
						cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH, 160 );                  // 16 lasers in one column
						cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT, 160 );                 // to get a square image
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
				
				
				IplImage* imgGreenThresh = GetThresholdedImage(img);                                    //Perform thresholding operation to get the binary image.
						
						GetImageMatrix(imgGreenThresh, LaserState, BLOCKSIZE, OCCUPANCYPERCENT);                //Get the LaserState Matrix generated from the thresholded image
						cvReleaseCapture (&capture);
						cvReleaseImage (&img);				
				NoOfPulses = 0;
			}

			ChangeLaserStates(LaserState, BufferLaser, Laser1, Laser2);
			for(i=0; i<8; i++)
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
			for(i=0; i<16; i++)
			{
				for(j=0; j<16; j++)
				{
					printf("%d ", LaserState[i][j]);
				}
				printf("\n");
			}
			printf("\n\n");
			
			NoOfPulses++;
			
		}
		
	}

	for(i=0; i<16; i++)
	{
		gpio_unexport(LaserGPIO[i]);
	}
	LS.Close();
	gpio_fd_close(gpio_fd);
	return 0;
}
