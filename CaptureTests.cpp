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

using namespace cv;

int main(int argc, char* argv[])
{
	int i=0;
	char c, Filename[10000]; 
	CvCapture* capture = NULL;
        IplImage* img = NULL;
 
	do		
	{
		capture = NULL; 
		img = NULL; 
		capture = cvCaptureFromCAM(-1);
		cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH, 160 );			// 16 lasers in one column
        	cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT, 160 );			// to get a square image
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
		if(!sprintf(Filename, "/home/root/Desktop/Laserweed/photos/%u.jpg", i))
		{
			std::cout << "Save image failed!" << std::endl; 
			break; 
		} 
		cvSaveImage(Filename,img);
		cvReleaseCapture( &capture );
		cvReleaseImage( &img );
		
		i++;
		c = getchar(); 
	}
	while(c!='e');
	return 0;
}





