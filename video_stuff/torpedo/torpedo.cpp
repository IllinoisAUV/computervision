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
void Dilation(int, void*);
void Erosion(int, void*);

int frame_count=0;

int main(){

	VideoCapture cap("red.mp4");
	if(!cap.isOpened()){
		cout << "Cannot open the video file. \n";
		return -1;
	}
   
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
		if(frame_count % 10 == 0){
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

	Mat src_contours = detected_edges;

	GaussianBlur(src_contours, src_contours, Size(3,3), 0, 0);
	imshow("blurred",src_contours);
	
	RNG rng(12345);
	Mat output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
		
	//findContours function
	findContours(src_contours, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0,0));

	//approximate the contours to get bounding rectangles and circles 
	vector<vector<Point> > contours_poly(contours.size());
	vector<Rect> boundRect(contours.size());
	vector<Point2f> center(contours.size());
	vector<float> radius(contours.size());
	
	for(size_t i = 0; i < contours.size(); i++){
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = boundingRect(Mat(contours_poly[i]));
		minEnclosingCircle((Mat)contours_poly[i], center[i], radius[i]);
	}


	vector<double> radii;
	for(int i = 0; i < contours.size(); i++){
		radii.push_back(radius[i]);
	
	}
	double largest_radius = 0;
	double largest_radius2 = 0;
	int largest_idx1 = -1;
	int largest_idx2 = -1;
	int largest_idx3 = -1;
	
	sort(contours);	
	for(size_t i = 0; i < radius.size(); i++){
		if(radius[i] > largest_radius){
			largest_idx3 = largest_idx2;
			largest_idx2 = largest_idx1;
			largest_idx1 = i;
			largest_radius2 = largest_radius;
			largest_radius = radius[i];
			indices.push_back(i);
		}
	//	cout << "center: " << center[i] << "       " << "radius: " << radius[i] << endl;
	}
//	indices.push_back(largest_idx1);
//	indices.push_back(largest_idx2);
//	indices.push_back(largest_idx3);


	Mat drawing = Mat::zeros(src_contours.size(), CV_8UC3);
	for(size_t i = 0; i < indices.size(); i++){
		int idx = indices[i];
		Scalar color = Scalar(rng.uniform(0,255), rng.uniform(0,255), rng.uniform(0,255));
		drawContours(drawing, contours_poly, idx, color, 1, 8, vector<Vec4i>(), 0, Point());
		//rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
		//circle(drawing, center[idx], (int)radius[idx], color, 2, 8, 0);
	}
	
	namedWindow("Color Contours", WINDOW_NORMAL);
	resizeWindow("Color Contours", 600,600);
	imshow("Color Contours", drawing);
}

