/*
 * LaserWeed.cpp
 * Author: sudhir vyasaraja
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <cxcore.h>
#include "LaserWeed.h"
#include "SimpleGPIO.h"
#define BLOCKSIZE 10														//N in a NxN pixel block for processing-depends on image resolution
#define OCCUPANCYPERCENT 25													//Percent of cell covered by vegetation
#define NOOFPULSES 2000*(4096/(4*3.1412*4))											//No of pulses for every 0.25 inch travelled

using namespace cv;

unsigned int LaserGPIO[16];

/*************************************************************************************************************/
//Main Function
int main(int argc, char* argv[])
{
	int i, j, count, NoOfPulses=0;
	static int LaserState[16][16];												//Create 16x16 matrix to hold the laser state values.
	static unsigned int Laser1[8], Laser2[8], BufferLaser[8]; 						
	char c, Filename[10000]; 
	int image_num=0;
        CvCapture* capture = NULL;
        IplImage* img = NULL;
      
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
		IplImage* imgGreenThresh = GetThresholdedImage(img);					//Perform thresholding operation to get the binary image.
                sprintf(Filename, "/home/root/Desktop/Laserweed/thresholdphotos/%u.jpg", image_num);
                if(!cvSaveImage(Filename,imgGreenThresh))
                {
                        std::cout << "Save image failed! Check code!" << std::endl;
                }
		GetImageMatrix(imgGreenThresh, LaserState, BLOCKSIZE, OCCUPANCYPERCENT);		//Get the LaserState Matrix generated from the thresholded image
		cvReleaseCapture (&capture); 
		cvReleaseImage (&img); 

/******************************************************************************************************************/

	while(1)
	{
		if(NoOfPulses == 10)
		{

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
			image_num++; 
	                sprintf(Filename, "/home/root/Desktop/Laserweed/photos/%u.jpg", image_num);
			if(!cvSaveImage(Filename,img))			
	                {
	                        std::cout << "Save image failed!" << std::endl;
	                        break;
	                }
			IplImage* imgGreenThresh = GetThresholdedImage(img);                                    //Perform thresholding operation to get the binary image.
	                sprintf(Filename, "/home/root/Desktop/Laserweed/thresholdphotos/%u.jpg", image_num);
        	        if(!cvSaveImage(Filename,imgGreenThresh))
                	{
                        	std::cout << "Save image failed! Check code!" << std::endl;
                	}
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
		for(i=0; i<8; i++)
		{
			printf("%d       ", Laser1[i]);
		}
		printf("\n");
		for(i=0; i<8; i++)
		{
			printf("    %d   ", Laser2[i]);
		}
		NoOfPulses++;
		getchar();
	}

	for(i=0; i<16; i++)
	{
		gpio_unexport(LaserGPIO[i]);
	}

	return 0;
}
