//
//  laserweed.h
//  openCV
//
//  Created by Rohit Garg on 6/22/15.
//  Copyright (c) 2015 Rohit Garg. All rights reserved.
//

#ifndef __openCV__laserweed__
#define __openCV__laserweed__

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/ml.hpp"
#include "opencv2/imgcodecs.hpp"
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <cstdio>
#include <cmath>

using namespace std;
using namespace cv;
using namespace cv::ml;

class laserweed{
    
public:
    const int width = 160;
    const int height = 120;
    bool destroy;
    CvRect box;
    bool drawing_box;
    int classes;
    Mat* veg;
    Mat* soil;
    //Mat* weed;
    Mat histVeg;
    Mat histSoil;
    //Mat histWeed;
    RNG rng;
    IplImage *destination;
    
    laserweed();
    
    
    /**************** Threshold Calculation **********************/
public:
    void my_mouse_callback( int , int , int , int , void* ); // Taking mouse input of the screen
    void saveToVeg(IplImage* ); // File handling and Mat object creation for Crop class
    void saveToWeed(IplImage* ); // File handling and Mat object creation for Weed class
    void saveToSoil(IplImage* ); // File handling and Mat object creation for soil class
    void minMaxPixelVal(Mat); //To get the min max pxel value in a image
    Mat drawHist(Mat&); // To draw the histogram
    void drawHistMain(Mat ,int ); // According to input parameter record the input pixel values to different class
    void draw_box(IplImage* , CvRect); // Draw the box according to mouse input
    void drawHistHandle(char *); // Main handler for drawing histograms using keyboard and mouse input
    /*********************************************************/
    void imgScan(Mat & );// To perform the pixel by pixel image scanning
    void displayChannels(Mat); // To display a particular section of the image
    Mat getInrange(Mat,int, int, int, int , int, int);// To get the binary image in a particular range
    Mat frameSubstraction(Mat ,Mat ); // To get a substracted image of consecutive frame
    void featureExtract(char *); // TO extract features like area, arc length
    Mat matchTemplate(Mat ,Mat ); // Match the source image to template image
    void startVideo(); // To get video output
    void compareHist(Mat,Mat);
    
};

class laserSVM{
public:
    bool plotSupportVectors;
    int numTrainingPoints;
    int numTestPoints;
    int size;
    int eq;
    
    laserSVM();
    float evaluate(Mat&,Mat&);
    void plot_binary(Mat&,Mat&,string);
    Mat labelData(Mat, int);
    void svm(Mat&,Mat&,Mat&,Mat&);
    int f(float,float,int);
    
};

#endif /* defined(__openCV__laserweed__) */

