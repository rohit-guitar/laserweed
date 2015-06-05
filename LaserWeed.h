/*
 * LaserWeed.h
 *  Author: sudhir vyasaraja
 */

#ifndef LASERWEED_H_
#define LASERWEED_H_

IplImage* GetThresholdedImage(IplImage*);
IplImage* InvertImage(IplImage*);
IplImage* DisplayImage(IplImage*);
void GetImageMatrix(IplImage*, int(*)[16], int, int, int, int);
void ChangeLaserStates(int(*)[16], unsigned int*, unsigned int*, unsigned int*, int);

#endif
