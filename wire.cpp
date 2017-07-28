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
void findWires(Mat &src_colored);

int frame_count=0;

int main(){

	VideoCapture cap("test.mp4");
	if(!cap.isOpened()){
		cout << "Cannot open the video file. \n";
		return -1;
	}
   
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
			resize(frame, frame, Size(400,400));		
			//stringstream ss;
			//ss << frame_count;
			//string str = ss.str();	
			//imwrite("test" + str + ".png", frame);

			Mat colored_img = color(frame);
			//moments(colored_img);
			imshow("hsv", colored_img);
			findWires(colored_img);
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
	blur(src_hsv,src_hsv,Size(3,3),Point(-1,-1));
	
	erode(src_hsv, src_hsv, getStructuringElement(MORPH_ELLIPSE, Size(3,3)));
	erode(src_hsv, src_hsv, getStructuringElement(MORPH_ELLIPSE, Size(3,3)));
	dilate(src_hsv, src_hsv, getStructuringElement(MORPH_ELLIPSE, Size(31,31)));
	
	cvtColor(src, src_hsv, COLOR_BGR2HSV);
	
	int lower_bound1, lower_bound2;
	int upper_bound1, upper_bound2;
	string colorToDetect;
	Mat lower_bound_image, upper_bound_image;
	lower_bound1 = 18;
	lower_bound2 = 60;
	upper_bound1 = 18;
	upper_bound2 = 60;

	inRange(src_hsv, Scalar(lower_bound1,50,70), Scalar(lower_bound2,255,255), lower_bound_image);
	inRange(src_hsv, Scalar(upper_bound1,50,70), Scalar(upper_bound2,255,255), upper_bound_image);

	addWeighted(lower_bound_image, 1.0, upper_bound_image, 1.0, 0.0, src_colored);	
	dilate(src_hsv, src_hsv, getStructuringElement(MORPH_ELLIPSE, Size(31,31)));
	return src_colored;
}

vector<double> findWires(Mat &src_colored){
	//use contour area to find the biggest contour
	Mat threshold_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	RNG rng(12345);
	int thresh = 100;
	int max_thresh = 255;

	/// Detect edges using Threshold
	threshold( src_colored, threshold_output, thresh, 255, THRESH_BINARY );
	/// Find contours
	findContours( threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

	if(contours.size() < 3){
		return;
	}

	/// Find the rotated rectangles and ellipses for each contour
	vector<RotatedRect> minRect( contours.size() );
	vector<RotatedRect> minEllipse( contours.size() );

	//map the areas with the indices
	vector<double> maxContours;
	map<double, int> contourMap;
	for(int i = 0; i < contours.size(); i++){
		contourMap[contourArea(contours[i])] = i;
		maxContours.push_back(contourArea(contours[i]));
	}

	sort(maxContours.begin(), maxContours.end());
	//get the 3 largest areas
	//make array of largest points
	reverse(maxContours.begin(), maxContours.end());

	double big1 = maxContours[0];
	double big2 = maxContours[1];
	double big3 = maxContours[2];
	
	cout << "first = " << big1 << "	second = " << big2 << "		third = " << big3 << endl;

	//get the 3 largest indices
	int idx1 = contourMap[big1];
	int idx2 = contourMap[big2];
	int idx3 = contourMap[big3];
	int minSizeContour = 150;
	int sizeOfNewContours = 0;
	if(big1 > minSizeContour){
		sizeOfNewContours++;
	}
	if(big2 > minSizeContour){
		sizeOfNewContours++;
	}
	if(big3 > minSizeContour){
		sizeOfNewContours++;
	}

	vector<vector<Point> > newContours;
	newContours.resize(sizeOfNewContours);

		if(big1 > minSizeContour){
				for(int i = 0; i < contours[idx1].size(); i++){
				newContours[0].push_back(contours[idx1][i]);
			}
		}
		if(big2 > minSizeContour){
			for(int i = 0; i < contours[idx2].size(); i++){
				newContours[1].push_back(contours[idx2][i]);
			}
		}
		if(big3 > minSizeContour){
			for(int i = 0; i < contours[idx3].size(); i++){
				newContours[2].push_back(contours[idx3][i]);
			}
		}

	for( int i = 0; i < newContours.size(); i++ )
	   { minRect[i] = minAreaRect( Mat(newContours[i]) );
       if( newContours[i].size() > 5 )
         { minEllipse[i] = fitEllipse( Mat(newContours[i]) ); }
     }

     double centerX = getCenter(newContours[0]);

     return outputAngle(threshold_output, centerX);

  // /// Draw contours + rotated rects + ellipses
	 // Mat drawing = Mat::zeros( threshold_output.size(), CV_8UC3 );
  // for( int i = 0; i< newContours.size(); i++ )
  //    {
  //      Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
  //      // contour
  //      drawContours( drawing, newContours, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
  //      // ellipse
  //      //ellipse( drawing, minEllipse[i], color, 2, 8 );
  //      // rotated rectangle
  //      Point2f rect_points[4]; minRect[i].points( rect_points );
  //      for( int j = 0; j < 4; j++ )
  //         line( drawing, rect_points[j], rect_points[(j+1)%4], color, 1, 8 );
  //    }

  // // Show in a window
  // namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
  // imshow( "Contours", drawing );
}


double getCenter(vector<Point> contours){
	double minx, maxx;
	if(contours.size()<1)
		return -1;
	minx = contours[0].getX();
	maxx = minx;
	double outY = contours[0].getY();
	for (int i = 0; i<contours.size(); i++)
	{
		if(contours[i].getX()>maxx)
			maxx = contours[i].getX();
		else if(contours[i].getX()<minx)
			minx = contours[i].getX();
	}
	double dist = 0.5 * (maxx-minx);
	outY -= dist;
	return (minx+maxx)/2;

}

vector<double> outputAngle(Mat src, double inputX){
	//using the colored image
	//we have a point, which is the center of a cirlce, should only have 1.
	int midX = src.size().width / 2;
	int midY = src.size().height / 2;
	int targetx = -1, targety = -1;
	targetX = inputX;
	y = midY;

	//now we know what point  we are going to travel to, so we want to calculate the angle

	//triangle
	//	    adj
	//       --------
	//	  \	|
	//	   \	|
	//	    \	|     opp			
	//	     \	|
	//	      \	|
	//	       \|

	//	cout << "target x = " << targetx << " target y = " << targety << endl;
	//	cout << "midx = " << midX << " midy = " << midY << endl;

	if(targetx == -1 && targety == -1){
		cout << "Nothing detected, Angle to travel : -1" << endl;
		return;
	}	

	float adj = abs(targetx - midX);
	float opp = abs(targety - midY);
	angle = atan(opp / adj);

	radius = sqrt(adj * adj + opp * opp);
	vector<double> res;
	res.push_back(angle);
	res.push_back(radius);
	res.push_back(-1);
	int quadrant;
	if(targety < midY && targetx > midX){
		quadrant = 1;
		cout << "Angle to travel : " << angle;
		cout <<  "	" << radius << endl;
		return res;
	}
	else if(targety < midY && targetx < midX){
		quadrant = 2;
		cout << "Angle to travel : " << 3.14159 - angle;
		cout <<  "	" << radius << endl;
		res[0] = 3.14159 - angle;
		return res;
	}
	else if(targety > midY && targetx < midX){
		quadrant = 3;
		cout << "Angle to travel : " << 3.14159 + angle;
		cout <<  "	" << radius << endl;
		res[0] = 3.14159 + angle;
		return res;
	}
	else{
		quadrant = 4;
		cout << "Angle to travel : " << 3.14159 + 1.5707 + angle;
		cout <<  "	" << radius << endl;
		res[0] = 3.14159 + 1.5707 + angle;
		return res;
	}
	
}





