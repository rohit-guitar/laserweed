/*
 * ImageOperations.cpp
 *
 *  Created on: Aug 1, 2013
 *  Author: sudhir vyasaraja
 */

#include <stdio.h>
#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <cxcore.h>

/*_________________________________________________________________________________________________*/
/* http://colorizer.org/ 
http://www.yafla.com/yaflacolor/ColorRGBHSL.html
http://www.color-blindness.com/color-name-hue/ */


//Performs the thresholding operation on the image and returns a binary image
IplImage* GetThresholdedImage(IplImage* img)
{
	// Convert the image into an HSV image
	IplImage* imgHSV = cvCreateImage(cvGetSize(img), 8, 3);
	cvCvtColor(img, imgHSV, CV_BGR2HSV);
		IplImage* imgThreshed = cvCreateImage(cvGetSize(img), 8, 1);
	cvInRangeS(imgHSV, cvScalar(18,26,64), cvScalar(90,133,204), imgThreshed);
	cvReleaseImage(&imgHSV);
	return imgThreshed;
}
//Thresholding operation code ends
/*_________________________________________________________________________________________________*/

/*_________________________________________________________________________________________________*/

//Function displays the image. Waits for the user to close the window
//and then destroys the image
IplImage* DisplayImage(IplImage* img)
{
	cvNamedWindow( "Example1", CV_WINDOW_AUTOSIZE );
	cvShowImage("Example1", img);
	cvWaitKey(0);
	cvReleaseImage( &img );
	cvDestroyWindow( "Example1" );
}
//DisplayImage code ends
/*_________________________________________________________________________________________________*/

/*_________________________________________________________________________________________________*/

void GetImageMatrix(IplImage* img, int(*LaserState)[16],int BlockSize_Height,int BlockSize_Width,int CellOccupancy, int Count_For_Rows)
{

	/*Code to divide the image into smaller fragments using ROI and
	processing individually*/
	int N =  BlockSize_Height;
	int M =  BlockSize_Width;
	float AreaOccupied = (255*CellOccupancy)/100;
	IplImage* imgDummy = cvCreateImage(cvGetSize(img), 8, 1);	//Create a dummy image of the same size as thresholded image
	cvCopy(img, imgDummy);						//Copy the thresholded image for further operations
	CvRect roi;							//Rectangular ROI
	CvSize size;
	int r, c, i, j;
	int count = 0;
	CvScalar meanValue;					        // individual windows mean value
	double SingleMeanValue;

	size = cvGetSize(imgDummy);//returns image ROI, in other words, height and width of matrix
//Iteratively send the different ROIs for processing.
	for (i = Count_For_Rows, r = 0; r < size.height; i--, r += N)
	    for (j=0, c = 0; c < size.width; j++, c += M)
	    {
	    	count++;
	        roi.x = c;										//Iteratively set the ROI
	        roi.y = r;								        	//and loop through the whole image
	        roi.width = (c + M > size.width) ? (size.width - c) : M;      	                      //in blocks to perform the required operation.
	        roi.height = (r + N > size.height) ? (size.height - r) : N;
	        cvSetImageROI(imgDummy, roi);
	        meanValue = cvAvg(imgDummy);
	        SingleMeanValue = meanValue.val[0];		//Extract the mean value of the block. This helps to decide how many pixels are white.
	        if(SingleMeanValue > AreaOccupied)		//Set the value in LaserState matrix depending on the
	        	LaserState[i][j] = 1;		        //cell occupancy percentage
			else
				LaserState[i][j] = 0;
	        //cvResetImageROI(imgDummy);

	    }
	cvResetImageROI(imgDummy);						//Do not forget to reset ROIs
}
//GetImageMatrix code ends
/*_________________________________________________________________________________________________*/


/*_________________________________________________________________________________________________*/

//Performs inversion operation on the image. Currently not using.
IplImage* InvertImage(IplImage* img)
{
	int height,width,step,channels;
	uchar *data;
	int i,j,k;

	height    = img->height;
	width     = img->width;
	step      = img->widthStep;
	channels  = img->nChannels;
	data      = (uchar *)img->imageData;

	// invert the image
	for(i=0;i<height;i++)
		for(j=0;j<width;j++)
			for(k=0;k<channels;k++)  //loop to read for each channel
				data[i*step+j*channels+k]=255-data[i*step+j*channels+k];    //inverting the image

	return img;
}
//Invert Image code ends
/*_________________________________________________________________________________________________*/




