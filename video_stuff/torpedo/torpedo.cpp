#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "highgui.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <string>
#include <sstream>
#include <cv.h>

using namespace std;
using namespace cv;

Mat color(Mat &src);
void moments(Mat &src_hsv);

int frame_count=0;

int main(){

	VideoCapture cap("red.mp4");
	if(!cap.isOpened()){
		cout << "Cannot open the video file. \n";
		return -1;
	}
   
//	double fps = cap.get(CV_CAP_PROP_FPS); //
	vector<vector<Point> > squares;

	namedWindow("source video", WINDOW_NORMAL); //create a window 
	resizeWindow("source video", 4000, 4000);

	while(1){
		Mat frame;
        if (!cap.read(frame)){ // if not success, break loop
        // read() decodes and captures the next frame.
			cout<<"\n Cannot read the video file. \n";
   		    break;
    	}
		frame_count++;
		if(frame_count % 1 == 0){
			Mat colored_img = color(frame);
			moments(colored_img);
			imshow("hsv", colored_img);
		}
		//imshow("colored", colored_img);
	    imshow("source video", frame);
		if(waitKey(30) == 27){ // 'esc' to quit
       		break; 
     	}
    }
    return 0;
}

Mat color(Mat &src){
	Mat src_hsv, src_colored;
	cvtColor(src, src_hsv, COLOR_BGR2HSV);
	int lower_bound1, lower_bound2;
	int upper_bound1, upper_bound2;
	string colorToDetect;
	Mat lower_bound_image, upper_bound_image;
	lower_bound1 = 160;
	lower_bound2 = 179;
	upper_bound1 = 160;
	upper_bound2 = 179;

	inRange(src_hsv, Scalar(lower_bound1,100,100), Scalar(lower_bound2,255,255), lower_bound_image);
	inRange(src_hsv, Scalar(upper_bound1,100,100), Scalar(upper_bound2,255,255), upper_bound_image);

	addWeighted(lower_bound_image, 1.0, upper_bound_image, 1.0, 0.0, src_colored);	
	return src_colored;
}


void moments(Mat &src_hsv){
	
	int low_threshold = 100;
	Mat detected_edges;
	blur(src_hsv, detected_edges, Size(3,3));
	Canny(detected_edges, detected_edges, low_threshold, 3 * low_threshold, 3);
	imshow("canny edges", detected_edges);	

/*	blur( src_gray, src_gray, Size(3,3) );
	threshold(src_gray, src_gray, 245, 255, 3);
	int thresh = 100; int max_thresh = 255; RNG rng(12345);
	Mat canny_output;	
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	Canny( src_gray, canny_output, thresh, thresh*2, 3 );
	findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

	vector<Moments> mu(contours.size() );
	for( int i = 0; i < contours.size(); i++ )
	{ mu[i] = moments( contours[i], false ); }

  ///  Get the mass centers:
  vector<Point2f> mc( contours.size() );
  for( int i = 0; i < contours.size(); i++ )
     { mc[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 ); }

  /// Draw contours
  Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
  for( int i = 0; i< contours.size(); i++ )
     {
		Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
       drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
       circle( drawing, mc[i], 4, color, -1, 8, 0 );
     }
	cout << "size" << contours.size() << endl;
*/

/*	IplImage* src_h = src_hsv.clone();
	IplImage* drawing = src_hsv.clone();
	CvSeq* contours;
	CvSeq* result;	
	CvMemStorage *storage = cvCreateMemStorage(0);
	cvFindContours(src_h, storage, &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));

	while(contours){
		result = cvApproxPoly(contours, sizeof(CvContour), storage, CV_POLY_APPROX_DP, cvContourPerimeter(contours)*0.02, 0);
		if(result->total == 6){
			CvPoint* pt[6];
			for(int i =0; i < 6; i++){
				pt[i] = (CvPoint*)cvGetSeqElem(result,i);
			}
			cvLine(drawing, *pt[0], *pt[1], cvScalar(0,255,0),4);
			cvLine(drawing, *pt[1], *pt[2], cvScalar(0,255,0),4);
			cvLine(drawing, *pt[2], *pt[3], cvScalar(0,255,0),4);
			cvLine(drawing, *pt[3], *pt[4], cvScalar(0,255,0),4);
			cvLine(drawing, *pt[4], *pt[5], cvScalar(0,255,0),4);
			cvLine(drawing, *pt[5], *pt[6], cvScalar(0,255,0),4);
		}
	}

*/	

	
	//imshow("targets", drawing);
}















