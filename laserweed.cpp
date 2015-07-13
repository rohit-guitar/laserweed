//
//  laserweed.cpp
//  openCV
//
//  Created by Rohit Garg on 6/22/15.
//  Copyright (c) 2015 Rohit Garg. All rights reserved.
//

#include "laserweed.h"

laserweed::laserweed(){
    destroy=false;
    drawing_box = false;
    classes = -1;
    soil= new Mat(width, height, CV_8UC3, Scalar(0,0,0));
    //crop= new Mat(width, height, CV_8UC3, Scalar(0,0,0));
    veg= new Mat(width, height, CV_8UC3, Scalar(0,0,0));
    rng(12345);
}


void laserweed::my_mouse_callback(int event, int x, int y, int flags, void* param){
    IplImage* frame = (IplImage*) param;
    
    switch( event )
    {
        case CV_EVENT_MOUSEMOVE:
        {
            if( drawing_box )
            {
                this->box.width = x-this->box.x;
                this->box.height = y-this->box.y;
            }
        }
            break;
            
        case CV_EVENT_LBUTTONDOWN:
        {
            this->drawing_box = true;
            this->box = cvRect( x, y, 0, 0 );
        }
            break;
            
        case CV_EVENT_LBUTTONUP:
        {
            this->drawing_box = false;
            if( this->box.width < 0 )
            {
                this->box.x += box.width;
                this->box.width *= -1;
            }
            
            if( this->box.height < 0 )
            {
                this->box.y += this->box.height;
                this->box.height *= -1;
            }
            
            if(this->classes==0){
                this->saveToSoil(frame);
            }
            else if(classes==1){
                this->saveToVeg(frame);
            }
           // else if(classes==2){
           //     this->saveToWeed(frame);
           // }
            
            
            
            this->draw_box(frame, box);
        }
            break;
            
        case CV_EVENT_RBUTTONUP:
        {
            this->destroy=true;
            
        }
            break;
            
        default:
            break;
    }
}

void laserweed::saveToVeg(IplImage* img){
    ofstream myfile;
    myfile.open("Veg.txt",ios::app);
    if (!myfile.is_open())
    {
        return;
    }
    //cout<<this->box.x <<" "<<this->box.x+box.width<<" "<<this->box.y<<" "<<this->box.y+this->box.height<<endl;
    for(int i=this->box.x;i<=this->box.x+this->box.width;i++){
        for(int j=this->box.y;j<=this->box.y+this->box.height;j++){
            CvScalar s= cvGet2D(img,j,i);
            myfile<<s.val[2]<<" "<<s.val[1]<<" "<<s.val[0]<<endl;
            Vec3b &data = this->veg->at<Vec3b>(j,i);
            data[0] = s.val[0];
            data[1] = s.val[1];
            data[2] = s.val[2];
        }
    }
    
    myfile.close();
}

/*void laserweed::saveToWeed(IplImage* img){
    ofstream myfile;
    myfile.open("weed.txt",ios::app);
    if (!myfile.is_open())
    {
        return;
    }
    cout<<this->box.x <<" "<<this->box.x+box.width<<" "<<this->box.y<<" "<<this->box.y+this->box.height<<endl;
    for(int i=this->box.x;i<=this->box.x+this->box.width;i++){
        for(int j=box.y;j<=box.y+box.height;j++){
            CvScalar s= cvGet2D(img,j,i);
            myfile<<s.val[2]<<" "<<s.val[1]<<" "<<s.val[0]<<endl;
            Vec3b &data = this->weed->at<Vec3b>(j,i);
            data[0] = s.val[0];
            data[1] = s.val[1];
            data[2] = s.val[2];
        }
    }
    
    myfile.close();
}*/

void laserweed::saveToSoil(IplImage* img){
    ofstream myfile;
    myfile.open("soil.txt",ios::app);
    if (!myfile.is_open())
    {
        return;
    }
    //cout<<box.x <<" "<<box.x+box.width<<" "<<box.y<<" "<<box.y+box.height<<endl;
    for(int i=this->box.x;i<=this->box.x+this->box.width;i++){
        for(int j=this->box.y;j<=this->box.y+this->box.height;j++){
            CvScalar s= cvGet2D(img,j,i);
            myfile<<s.val[2]<<" "<<s.val[1]<<" "<<s.val[0]<<endl;
            Vec3b &data = this->soil->at<Vec3b>(j,i);
            data[0] = s.val[0];
            data[1] = s.val[1];
            data[2] = s.val[2];
        }
    }
    
    myfile.close();
}

void laserweed::minMaxPixelVal(Mat img){
    // HSV
     Mat des;
    cvtColor(img, des, COLOR_BGR2YCrCb);
     //cvtColor(img, des, CV_BGR2HSV);
     //namedWindow("HSV", WINDOW_AUTOSIZE );
    
    vector<Mat> rgbChannels(3);
    split(des, rgbChannels);
    
    double RminVal; double RmaxVal; Point RminLoc; Point RmaxLoc;
    
    minMaxLoc( rgbChannels[2], &RminVal, &RmaxVal, &RminLoc, &RmaxLoc);
    cout<<"RMinimum value:- "<<RminVal <<"  RMaximum value:- "<<RmaxVal;
    double GminVal; double GmaxVal; Point GminLoc; Point GmaxLoc;
    minMaxLoc( rgbChannels[1], &GminVal, &GmaxVal, &GminLoc, &GmaxLoc);
    cout<<"GMinimum value:- "<<GminVal <<"  GMaximum value:- "<<GmaxVal;
    
    double BminVal; double BmaxVal; Point BminLoc; Point BmaxLoc;
    minMaxLoc( rgbChannels[0], &BminVal, &BmaxVal, &BminLoc, &BmaxLoc);
    cout<<"BMinimum value:- "<<BminVal <<"  BMaximum value:- "<<BmaxVal;
}

void laserweed::compareHist(Mat img1,Mat img2){
    if( img1.empty())
    { cout<<"Empty case";return; }
  
    vector<Mat> bgr_planes1,bgr_planes2;
    split(img1, bgr_planes1);
    split(img2, bgr_planes2);
    int histSize = 256;
    
    /// Set the ranges ( for B,G,R) )
    float range[] = { 0, 256 } ;
    const float* histRange = { range };
    
    bool uniform = true; bool accumulate = false;
    
    Mat b_hist1, g_hist1, r_hist1;
    Mat b_hist2, g_hist2, r_hist2;
    
    /// Compute the histograms:
    calcHist( &bgr_planes1[0], 1, 0, Mat(), b_hist1, 1, &histSize, &histRange, uniform, accumulate );
    calcHist( &bgr_planes1[1], 1, 0, Mat(), g_hist1, 1, &histSize, &histRange, uniform, accumulate );
    calcHist( &bgr_planes1[2], 1, 0, Mat(), r_hist1, 1, &histSize, &histRange, uniform, accumulate );
    
    /// Compute the histograms:
    calcHist( &bgr_planes2[0], 1, 0, Mat(), b_hist2, 1, &histSize, &histRange, uniform, accumulate );
    calcHist( &bgr_planes2[1], 1, 0, Mat(), g_hist2, 1, &histSize, &histRange, uniform, accumulate );
    calcHist( &bgr_planes2[2], 1, 0, Mat(), r_hist2, 1, &histSize, &histRange, uniform, accumulate );
    
    cout<< "Image1 R"<<r_hist1<<endl;
    cout<< "Image2 R"<<r_hist2<<endl;
    
    // Draw the histograms for B, G and R
    int hist_w = 520; int hist_h = 400;
    int bin_w = cvRound( (double) hist_w/histSize );
    
    Mat histImage1( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );
    Mat histImage2( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );
    Mat histImage3( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );
    
    /// Normalize the result to [ 0, histImage.rows ]
    normalize(b_hist1, b_hist1, 0, histImage1.rows, NORM_MINMAX, -1, Mat());
    normalize(b_hist2, b_hist2, 0, histImage1.rows, NORM_MINMAX, -1, Mat());

    
    normalize(g_hist1, g_hist1, 0, histImage2.rows, NORM_MINMAX, -1, Mat());
    normalize(g_hist2, g_hist2, 0, histImage2.rows, NORM_MINMAX, -1, Mat());

    
    normalize(r_hist1, r_hist1, 0, histImage3.rows, NORM_MINMAX, -1, Mat());
    normalize(r_hist2, r_hist2, 0, histImage3.rows, NORM_MINMAX, -1, Mat());

    
    /// Draw for each channel
    for( int i = 1; i < histSize; i++ )
    {
        line( histImage1, Point( bin_w*(i-1), hist_h -
                               cvRound(b_hist1.at<float>(i-1)) ) ,
             Point( bin_w*(i), hist_h - cvRound(b_hist1.at<float>(i)) ),
             Scalar( 255, 0, 0), 2, 8, 0  );
        line( histImage1, Point( bin_w*(i-1), hist_h -
                                cvRound(b_hist2.at<float>(i-1)) ) ,
             Point( bin_w*(i), hist_h - cvRound(b_hist2.at<float>(i)) ),
             Scalar( 150, 50, 50), 2, 8, 0  );
    }
    
    for( int i = 1; i < histSize; i++ )
    {
        line( histImage2, Point( bin_w*(i-1), hist_h -
                                cvRound(g_hist1.at<float>(i-1)) ) ,
             Point( bin_w*(i), hist_h - cvRound(g_hist1.at<float>(i)) ),
             Scalar( 0, 255, 0), 2, 8, 0  );
        line( histImage2, Point( bin_w*(i-1), hist_h -
                                cvRound(g_hist2.at<float>(i-1)) ) ,
             Point( bin_w*(i), hist_h - cvRound(g_hist2.at<float>(i)) ),
             Scalar( 100, 150, 100), 2, 8, 0  );
    }
    
    for( int i = 1; i < histSize; i++ )
    {
        line( histImage3, Point( bin_w*(i-1), hist_h -
                                cvRound(r_hist1.at<float>(i-1)) ) ,
             Point( bin_w*(i), hist_h - cvRound(r_hist1.at<float>(i)) ),
             Scalar( 0, 0, 255), 2, 8, 0  );
        line( histImage3, Point( bin_w*(i-1), hist_h -
                                cvRound(r_hist2.at<float>(i-1)) ) ,
             Point( bin_w*(i), hist_h - cvRound(r_hist2.at<float>(i)) ),
             Scalar( 100, 200,150), 2, 8, 0  );
    }
    
    namedWindow("Blue hist", WINDOW_AUTOSIZE );
    imshow("Blue hist", histImage1 );
    namedWindow("Green hist", WINDOW_AUTOSIZE );
    imshow("Green hist", histImage2 );
    namedWindow("Red hist", WINDOW_AUTOSIZE );
    imshow("Red hist", histImage3 );
}


Mat laserweed::drawHist(Mat& src){
    if( src.empty() )
    { return src; }
    
    /// Separate the image in 3 places ( B, G and R )
    vector<Mat> bgr_planes;
    split( src, bgr_planes );
    
    /// Establish the number of bins
    int histSize = 256;
    
    /// Set the ranges ( for B,G,R) )
    float range[] = { 0, 256 } ;
    const float* histRange = { range };
    
    bool uniform = true; bool accumulate = false;
    
    Mat b_hist, g_hist, r_hist;
    
    /// Compute the histograms:
    calcHist( &bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate );
    calcHist( &bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate );
    calcHist( &bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate );
    
    // Draw the histograms for B, G and R
    int hist_w = 520; int hist_h = 400;
    int bin_w = cvRound( (double) hist_w/histSize );
    
    Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );
    
    /// Normalize the result to [ 0, histImage.rows ]
    normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
    normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
    normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
    
    /// Draw for each channel
    for( int i = 1; i < histSize; i++ )
    {
        line( histImage, Point( bin_w*(i-1), hist_h -
                               cvRound(b_hist.at<float>(i-1)) ) ,
             Point( bin_w*(i), hist_h - cvRound(b_hist.at<float>(i)) ),
             Scalar( 255, 0, 0), 2, 8, 0  );
       line( histImage, Point( bin_w*(i-1), hist_h -
                               cvRound(g_hist.at<float>(i-1)) ) ,
             Point( bin_w*(i), hist_h - cvRound(g_hist.at<float>(i)) ),
             Scalar( 0, 255, 0), 2, 8, 0  );
        line( histImage, Point( bin_w*(i-1), hist_h -
                               cvRound(r_hist.at<float>(i-1)) ) ,
             Point( bin_w*(i), hist_h - cvRound(r_hist.at<float>(i)) ),
             Scalar( 0, 0, 255), 2, 8, 0  );
    }
    
    /// Display
    //namedWindow("calcHist Demo", WINDOW_AUTOSIZE );
    //imshow("calcHist Demo", histImage );
    return histImage;
}

void laserweed::drawHistMain(Mat in,int t){
    Mat thresImg;
    inRange(in, Scalar(1,1,1), Scalar(255,255,255), thresImg);
    //imshow("ThreshImg", thresImg);
    int nonZeroPixels = countNonZero(thresImg);
    int k=0;
    Mat newCrop = Mat(Size(nonZeroPixels, 1), CV_8UC3, Scalar(0, 0, 0));
    uchar r, g, b;
    for (int i = 0; i < in.rows; ++i)
    {
        Vec3b* pixel = in.ptr< Vec3b>(i); // point to first pixel in row
        for (int j = 0; j < in.cols; ++j)
        {
            Vec3b color = in.at<Vec3b>(Point(j,i));
            r = pixel[j][2];
            g = pixel[j][1];
            b = pixel[j][0];
            if(r==0 && b==0 && g==0){
                continue;
            }
            else{
                
                newCrop.at<Vec3b>(Point(k,0)) = color;
                k++;
            }
        }
    }
    //drawHist(crop);
    if(t==1){
        veg= new Mat(newCrop);
        minMaxPixelVal(newCrop);
        histVeg =drawHist(newCrop);
    }
    else if(t==2){
        soil = new Mat(newCrop);
        minMaxPixelVal(newCrop);
        histSoil = drawHist(newCrop);
    }
    //else if (t==3) histSoil = drawHist(newCrop);
}

void laserweed::draw_box(IplImage* img, CvRect rect){
    cvRectangle(img, cvPoint(this->box.x, this->box.y), cvPoint(this->box.x+this->box.width,this->box.y+this->box.height),
                cvScalar(0,0,255) ,2);
}


void setupCallback(int event, int x, int y, int flags, void* ptr){
    laserweed* hand = (laserweed*) ptr;
    if(hand!=NULL){
        hand->my_mouse_callback(event,x,y,flags,(void *)hand->destination);
    }
}

void laserweed::drawHistHandle(char* imagePath){
    const char* name = "Box Example";
    cvNamedWindow( name,CV_WINDOW_AUTOSIZE);
    this->box = cvRect(0,0,1,1);
    
    
    IplImage* image = cvLoadImage(imagePath);
    if (!image)
    {
        printf("!!! Failed cvQueryFrame #1\n");
        return ;
    }
    destination = cvCreateImage(cvSize((int)(width),(int)(height)),image->depth, image->nChannels );
    cvResize(image, destination,CV_INTER_LINEAR);
    //cvShowImage(name,destination);
    
    IplImage* temp = cvCloneImage(destination);
    
    // Set up the callback
    cvSetMouseCallback(name, setupCallback, (void *)this);
    remove("/Users/Rohit/Documents/projects/openCV/build/Debug/Veg.txt");
    //remove("/Users/Rohit/Documents/projects/openCV/build/Debug/crop.txt");
    remove("/Users/Rohit/Documents/projects/openCV/build/Debug/soil.txt");
    
    // Main loop
    while(1)
    {
        if (destroy)
        {
            cvDestroyWindow(name); break;
        }
        cvCopy(destination, temp);
        
        if (drawing_box)
            draw_box(temp, box);
        
        //cvMoveWindow(name, 200, 100);
        cvShowImage(name, temp);
        
        char key=cvWaitKey(15);
        
        if (key == 32){
            cout<<"Soil mode";
            classes=0;
        }
        
        if (key == 92){
            cout<<"Veg mode";
            classes=1;
        }
        
      /*  if (key == 47){
            cout<<"Weed mode";
            classes=2;
        }*/
        
        if (key == 27){
            drawHistMain(*veg,1);
            drawHistMain(*soil,2);
            compareHist(*veg,*soil);
            //drawHistMain(*soil,3);
            namedWindow("Crop Histogram", WINDOW_AUTOSIZE );
            imshow("Crop Histogram", histVeg );
            //namedWindow("Weed Histogram", WINDOW_AUTOSIZE );
            //imshow("Weed Histogram", histWeed );
            namedWindow("Soil Histogram", WINDOW_AUTOSIZE );
            imshow("Soil Histogram", histSoil );
           // cout<<"For Weed :-"<<endl;
            //minMaxPixelVal(*weed);
            
            break;
        }
    }
    
    
    cvReleaseImage(&temp);
    cvDestroyWindow(name);
}

void laserweed::imgScan(Mat & img){
    int rows = img.rows;
    int cols = img.cols;
    
    if (img.isContinuous())
    {
        cols = rows * cols; // Loop over all pixels as 1D array.
        rows = 1;
    }
    
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            Vec3b & color = img.at<Vec3b>(Point(i,j));
            if(color[0] > 150 && color[1] > 150 && color[2] > 150)
            {
                color[0] = 0;
                color[1] = 0;
                color[2] = 0;
                cout << "Pixel >200 :" << i << "," << j << endl;
            }
            else
            {
                color[0] = 255;
                color[1] = 255;
                color[2] = 255;
            }
        }
    }
    imshow("image", img);
    waitKey(0);
    destroyAllWindows();
    
}

void laserweed::displayChannels(Mat src){
    vector<Mat> rgbChannels(3);
    split(src, rgbChannels);
    
    // Show individual channels
    Mat g, fin_img;
    g = Mat::zeros(Size(src.cols, src.rows), CV_8UC1);
    
    // Showing Red Channel
    // G and B channels are kept as zero matrix for visual perception
    {
        vector<Mat> channels;
        channels.push_back(g);
        channels.push_back(g);
        channels.push_back(rgbChannels[2]);
        
        /// Merge the three channels
        merge(channels, fin_img);
        namedWindow("R",1);imshow("R", fin_img);
    }
    
    // Showing Green Channel
    {
        vector<Mat> channels;
        channels.push_back(g);
        channels.push_back(rgbChannels[1]);
        channels.push_back(g);
        merge(channels, fin_img);
        namedWindow("G",1);imshow("G", fin_img);
    }
    
    // Showing Blue Channel
    {
        vector<Mat> channels;
        channels.push_back(rgbChannels[0]);
        channels.push_back(g);
        channels.push_back(g);
        merge(channels, fin_img);
        namedWindow("B",1);imshow("B", fin_img);
    }
    
}

Mat laserweed::getInrange(Mat src,int rmin , int rmax, int gmin, int gmax, int bmin, int bmax){
    Mat thresImg;
    inRange(src, Scalar(bmin,gmin,rmin), Scalar(bmax,gmax,rmax), thresImg);
    //bitwise_not(thresImg, thresImg);
    return thresImg;
}

Mat laserweed::frameSubstraction(Mat prevImg,Mat currImg){
    Mat deltaImg;
    absdiff(currImg,prevImg,deltaImg);
    
    Mat grayscale;
    cvtColor(deltaImg, grayscale, CV_BGR2GRAY);
    
    Mat mask = grayscale>0;
    Mat output;
    currImg.copyTo(output,mask);
    return output;
}

Mat laserweed::matchTemplate(Mat templ,Mat img){
    Mat result;
    Mat img_display;
    img.copyTo( img_display );
    
    /// Create the result matrix
    int result_cols =  img.cols - templ.cols + 1;
    int result_rows = img.rows - templ.rows + 1;
    
    result.create( result_rows, result_cols, CV_32FC1 );
    
    /// Do the Matching and Normalize
    cv::matchTemplate( img, templ, result, CV_TM_SQDIFF );
    normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );
    
    /// Localizing the best match with minMaxLoc
    double minVal; double maxVal; Point minLoc; Point maxLoc;
    Point matchLoc;
    
    minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
    matchLoc = minLoc;
    
    /// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
    /*
     if( match_method  == TM_SQDIFF || match_method == TM_SQDIFF_NORMED
     )
     { matchLoc = minLoc; }
     else
     { matchLoc = maxLoc; }
     */
    
    /// Show me what you got
    rectangle( img_display, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );
    rectangle( result, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );
    
    imshow( "Image orignal", img_display );
    imshow( "Result Window", result );
    return result;
}

void laserweed::featureExtract(char* imagePath){
    Mat resized;
    Mat imh = imread(imagePath);
    resize(imh, resized, Size(width, height), 0, 0, INTER_CUBIC);
    Mat des;
    cvtColor(resized, des, CV_BGR2HSV);
    namedWindow("HSV", WINDOW_AUTOSIZE );
    imshow("HSV", resized );
    
    Mat thresImg;
    //Mat image1=resized.clone();//cloning image
    //GaussianBlur( resized, image1, Size( 3, 3), 0, 0 );//applying Gaussian filter
    inRange(des, Scalar(18,26,64), Scalar(90,133,204), thresImg);
    imshow("ThreshImg", thresImg);
    //imshow("Gaussian", image1);

    Mat newThresh;
    Mat canny_output; //example from OpenCV Tutorial

    Canny(thresImg, canny_output, 10, 10*3, 3);//with or without, explained later.
    imshow("Canny",canny_output);
    Size kernalSize (5,5);
    Mat element = getStructuringElement (MORPH_RECT, kernalSize, Point(1,1)  );
    morphologyEx( canny_output, canny_output, MORPH_CLOSE, element );
    //canny_output.convertTo(newThresh, CV_8U);
    imshow("New Thresh",canny_output);
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours(thresImg, contours, hierarchy, CV_RETR_TREE,CV_CHAIN_APPROX_SIMPLE, Point(0,0));
    
    /*vector<vector <Point> >::iterator it = contours.begin();
    for( int i = 0; i< contours.size(); i++ ){
        if(contourArea(contours[i])<9){
            contours.erase(it);
            
        }else{
            it++;
        }
    }*/
   
    for (vector<vector<Point> >::iterator it = contours.begin(); it!=contours.end(); )
    {
        
        if (it->size()<10)
            it=contours.erase(it);
        else
            ++it;
    }
    
    
    /// Approximate contours to polygons + get bounding rects and circles
    vector<vector<Point> > contours_poly( contours.size() );
    vector<Rect> boundRect( contours.size() );
    vector<Point2f>center( contours.size() );
    vector<float>radius( contours.size() );
    vector<double> area (contours.size());
    vector<double>perimeter (contours.size());
    vector<double> circularity (contours.size());
    vector<RotatedRect> minEllipse( contours.size() );
    vector<double> axisRatio (contours.size());
    
    for( int i = 0; i < contours.size(); i++ )
    {
        approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
        perimeter[i] = arcLength(Mat(contours[i]), true);
        area[i] = contourArea(Mat(contours[i]));
        circularity[i] = 4.f * 3.14f * area[i] / (perimeter[i] * perimeter[i]);
        boundRect[i] = boundingRect( Mat(contours_poly[i]) );
        minEnclosingCircle( (Mat)contours_poly[i], center[i], radius[i]);
        minEllipse[i] = fitEllipse( Mat(contours[i]) );
        double aspect = (double)boundRect[i].width / (double)boundRect[i].height;
        minEnclosingCircle( contours_poly[i], center[i], radius[i] );
        Size2f s = minEllipse[i].size;
        axisRatio[i]= min(s.height,s.width)/max(s.height,s.width);
        cout << "feature" << i << " Perieter:-" << perimeter[i] << ", Area:-" << area[i] << ", Circle:-"<< circularity[i] << ", BRectratio:-" << aspect<<", axisRatio:-"<<axisRatio[i]<<endl;
        
    }
    
    
    /// Draw polygonal contour + bonding rects + circles
    Mat drawing = Mat::zeros( thresImg.size(), CV_8UC3 );
    for( int i = 0; i< contours.size(); i++ )
    {
        int fontface = cv::FONT_HERSHEY_SIMPLEX;
        double scale = 0.5;
        int thickness = 1;
        int baseline = 0;
        Scalar color =  Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        string label = to_string(i);
        //if((radius[i]<60) && (circleness[i]<15)){
           // continue;
        //}
        //double ratio = area[i]/circularity[i];
        //if(circularity[i]>0.2)continue;
        //else{
            //drawContours( drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
            drawContours(drawing, contours, i, Scalar(255, 255, 255), CV_FILLED);
            //rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
            //circle( drawing, center[i], (int)radius[i], color, 2, 8, 0 );
            //putText(Mat(contours[i]), label, center[i], fontface, scale, CV_RGB(255,255,0), thickness, 8);

        //}
    }
    
    /// Show in a window
    namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
    imshow( "Contours", drawing );
    imwrite("0.jpg",drawing);
    //imshow( "Canny", canny_output );
    
    
    //vector<Point> contour = contours[i];
    // double area0 = contourArea(contour);
    
    
}

void laserweed::startVideo(){
    char filename[80];
    int i=0;
    VideoCapture cap(0); // open the video camera no. 0
    if (!cap.isOpened())  // if not success, exit program
    {
        cout << "Cannot open the video cam" << endl;
        return ;
    }
    
    double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
    double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video
    
    cout << "Frame size : " << dWidth << " x " << dHeight << endl;
    
    namedWindow("MyVideo",CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"
    
    while (1)
    {
        Mat frame;
        
        bool bSuccess = cap.read(frame); // read a new frame from video
        
        if (!bSuccess) //if not success, break loop
        {
            cout << "Cannot read a frame from video stream" << endl;
            break;
        }
        Mat dst;//dst image
        pyrUp( frame, dst, Size(frame.cols*2, frame.rows*2));
        
        imshow("MyVideo", dst);
        
        
        char key = waitKey(1);
        
        if(key == 32){
            cout << "space key is pressed by user" << endl;
            sprintf(filename,"/Users/Rohit/LaserUf/beagle/test_%d.jpg",i);
            imwrite(filename, dst);
            i++;
        }
        
        else if (key == 27)
        {
            cout << "esc key is pressed by user" << endl;
            break;
        }
    }
    
}

laserSVM::laserSVM(){
    plotSupportVectors = true;
    numTrainingPoints = 200;
    numTestPoints = 2000;
    size = 200;
    eq = 0;
}

int laserSVM::f(float x, float y, int equation) {
    switch(equation) {
        case 0:
            return y > sin(x*10) ? -1 : 1;
            break;
        case 1:
            return y > cos(x * 10) ? -1 : 1;
            break;
        case 2:
            return y > 2*x ? -1 : 1;
            break;
        case 3:
            return y > tan(x*10) ? -1 : 1;
            break;
        default:
            return y > cos(x*10) ? -1 : 1;
    }
}

float laserSVM::evaluate(Mat& predicted,Mat& actual){
    assert(predicted.rows == actual.rows);
    int t = 0;
    int f = 0;
    for(int i = 0; i < actual.rows; i++) {
        float p = predicted.at<float>(i,0);
        float a = actual.at<float>(i,0);
        if((p >= 0.0 && a >= 0.0) || (p <= 0.0 &&  a <= 0.0)) {
            t++;
        } else {
            f++;
        }
    }
    return (t * 1.0) / (t + f);
}

void laserSVM::plot_binary(Mat& data,Mat& classes, string name){
    Mat plot(size, size, CV_8UC3);
    plot.setTo(Scalar(255.0,255.0,255.0));
    for(int i = 0; i < data.rows; i++) {
        
        float x = data.at<float>(i,0) * size;
        float y = data.at<float>(i,1) * size;
        
        if(classes.at<float>(i, 0) > 0) {
            circle(plot, Point(x,y), 2, CV_RGB(255,0,0),1);
        } else {
            circle(plot, Point(x,y), 2, CV_RGB(0,255,0),1);
        }
    }
    imshow(name, plot);
}

Mat laserSVM::labelData(Mat points, int equation){
    Mat labels(points.rows, 1, CV_32FC1);
    for(int i = 0; i < points.rows; i++) {
        float x = points.at<float>(i,0);
        float y = points.at<float>(i,1);
        labels.at<float>(i, 0) = f(x, y, equation);
    }
    return labels;
}

/*void laserSVM::svm(Mat& trainingData, Mat& trainingClasses, Mat& testData, Mat& testClasses){
    //CvSVMParams param = CvSVMParams();
    SVM::Params params;
    params.svm_type = CvSVM::C_SVC;
    params.kernel_type = CvSVM::RBF; //CvSVM::RBF, CvSVM::LINEAR ...
    params.degree = 0; // for poly
    params.gamma = 20; // for poly/rbf/sigmoid
    param.coef0 = 0; // for poly/sigmoid
    
    param.C = 7; // for CV_SVM_C_SVC, CV_SVM_EPS_SVR and CV_SVM_NU_SVR
    param.nu = 0.0; // for CV_SVM_NU_SVC, CV_SVM_ONE_CLASS, and CV_SVM_NU_SVR
    param.p = 0.0; // for CV_SVM_EPS_SVR
    
    param.class_weights = NULL; // for CV_SVM_C_SVC
    param.term_crit.type = CV_TERMCRIT_ITER +CV_TERMCRIT_EPS;
    param.term_crit.max_iter = 1000;
    param.term_crit.epsilon = 1e-6;
    
    CvANN_MLP mlp;
    // SVM training (use train auto for OpenCV>=2.0)
    CvSVM svm(trainingData, trainingClasses, cv::Mat(), cv::Mat(), param);
    
    cv::Mat predicted(testClasses.rows, 1, CV_32F);
    
    for(int i = 0; i < testData.rows; i++) {
        cv::Mat sample = testData.row(i);
        
        float x = sample.at<float>(0,0);
        float y = sample.at<float>(0,1);
        
        predicted.at<float>(i, 0) = svm.predict(sample);
    }
    
    cout << "Accuracy_{SVM} = " << evaluate(predicted, testClasses) << endl;
    plot_binary(testData, predicted, "Predictions SVM");
    
    // plot support vectors
    if(plotSupportVectors) {
        cv::Mat plot_sv(size, size, CV_8UC3);
        plot_sv.setTo(cv::Scalar(255.0,255.0,255.0));
        
        int svec_count = svm.get_support_vector_count();
        for(int vecNum = 0; vecNum < svec_count; vecNum++) {
            const float* vec = svm.get_support_vector(vecNum);
            cv::circle(plot_sv, Point(vec[0]*size, vec[1]*size), 3 , CV_RGB(0, 0, 0));
        }
        cv::imshow("Support Vectors", plot_sv);
    }
}
*/

void newthreshold(char *in){
    Mat img =  imread(in); // from camera or some other source
    
    // STEP 1: learning phase
    Mat hsv, imgThreshed, processed, denoised;
    GaussianBlur(img, denoised, cv::Size(5,5), 2, 2); // remove noise
    cvtColor(denoised, hsv, CV_BGR2HSV);
    
    // lets say we picked manually a region of 100x100 px with the interested color/object using mouse
   //Mat roi = hsv (Range(mousex-50, mousey+50), Range(mousex-50, mousey+50));
    
    // must split all channels to get Hue only
    std::vector<cv::Mat> hsvPlanes;
    //cv::split(roi, hsvPlanes);
    
    // compute statistics for Hue value
    cv::Scalar mean, stddev;
    cv::meanStdDev(hsvPlanes[0], mean, stddev);
    
    // ensure we get 95% of all valid Hue samples (statistics 3*sigma rule)
    float minHue = mean[0] - stddev[0]*3;
    float maxHue = mean[0] + stddev[0]*3;
    
    // STEP 2: detection phase
    cv::inRange(hsvPlanes[0], cv::Scalar(minHue), cv::Scalar(maxHue), imgThreshed);
    imshow("thresholded", imgThreshed);
    
    //cv_erode(imgThreshed, processed, 5);  // minimizes noise
    //cv_dilate(processed, processed, 20);  // maximize left regions
    
    imshow("final", processed);

}

void conYcrcb(Mat img){
    Mat YUV444, GRB;
    vector<Mat> SplitYCbCr;
    cvtColor(img, YUV444, COLOR_BGR2YCrCb);
    split(YUV444, SplitYCbCr);
    Mat des,threshImg;
    inRange(SplitYCbCr[2], 97,133, threshImg);
    imshow("Thresh", threshImg);
}

int main(void){
    laserweed hand;
    //hand.featureExtract();
    char * input = "/Users/Rohit/LaserUf/new/0.jpg";
    //hand.displayChannels(imread(input));
    /*vector<Mat> bgr_planes;
    split( imread(input), bgr_planes );
    imshow("R",bgr_planes[2]);
    imshow("G",bgr_planes[1]);
    imshow("B",bgr_planes[0]);*/
    //Mat des,threshImg;
    //cvtColor(imread(input), des, CV_BGR2HSV);
    //inRange(des, Scalar(0,0,165), Scalar(27,35,255), threshImg);
    //imshow("Thresh", threshImg);
    hand.featureExtract(input);
    //hand.drawHistHandle(input);
    //conYcrcb(imread(input));
    waitKey(0);
    //destroyAllWindows();
    return 0;
    
}

