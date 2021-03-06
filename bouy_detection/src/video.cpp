//#include <opencv/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
//#include "opencv2/videoio.hpp"
#include <time.h>
#include <ros/ros.h>
#include <std_msgs/MultiArrayLayout.h>
#include <std_msgs/MultiArrayDimension.h>
#include <std_msgs/Float64MultiArray.h>
#include <std_msgs/Float32.h>

using namespace std;
using namespace cv;


#define PI 3.14159265

Mat detect_object(Mat src);
void outputAngle(Mat src, vector<Point2f> candidates);

int frameCount = 0;
float angle;
float radius;
int main(int argc, char **argv)
{

	ros::init(argc, argv, "buoy_publisher");
	ros::NodeHandle n;
	ros::Publisher velocity_pub = n.advertise<std_msgs::Float32>("/velocity", 1);
	ros::Publisher yaw_pub = n.advertise<std_msgs::Float32>("/yaw" , 1);
	
	VideoCapture cap("/home/ubuntu/catkin_ws/src/bouy_detection/src/underwatertest.mp4");
	if ( !cap.isOpened() ){
		cout << "Cannot open the video file. \n";
		return -1;
	}
	
	//VideoCapture cap(0);

	float fps = cap.get(CV_CAP_PROP_FPS); //

	namedWindow("source video", WINDOW_NORMAL); //create a window 
	resizeWindow("source video", 7000, 7000);

	ros::Rate loop_rate(10);

	while( ros::ok() ){
		Mat frame;

		if (!cap.read(frame)){ // if not success, break loop
			// read() decodes and captures the next frame.
			cout << "\n Cannot read the video file. \n";
			break;
		}

		if(frameCount % 5 == 0){
			Mat coloredImage = detect_object(frame);	//want this to occur on an interval maybe .5 secs
			imshow("colored", coloredImage);

			std_msgs::Float32 velocity;
			std_msgs::Float32 yaw;

			//Clear array
			//velocity.data.clear();
			//yaw.data.clear();
		
			velocity.data = 0.3;
			
			float weight = 20 ;
			
			yaw.data = (weight) * angle/PI  ;
			
			if (  yaw.data > 0.5  )
				yaw.data = 0.5;

	

			//Publish yaw and velocity
			velocity_pub.publish(velocity);
			yaw_pub.publish( yaw );
			
			


			if( frameCount < 1500 ) 
				ROS_INFO( "Nothing detected " );
			else 
				ROS_INFO( "%f : velocity , %f : yaw" , velocity.data , yaw.data   );


			//Do this.
			//ros::spinOnce();
			
			//Added a delay so not to spam
			//sleep(2);

		}

		ros::spinOnce();

		loop_rate.sleep();

		//imshow("colored", coloredImage);
		imshow("source video", frame);

		if(waitKey(30) == 27) //'esc' key to exit
		{ 
			break; 
		}

		frameCount++;
		cout << frameCount << endl;
	}

	return 0;
}

Mat detect_object(Mat src){
	Mat src_hsv, src_colored;
	cvtColor(src, src_hsv, COLOR_BGR2HSV);
	int lower_bound1, lower_bound2;
	int upper_bound1, upper_bound2;
	string colorToDetect;
	Mat lower_bound_image, upper_bound_image;
	lower_bound1 = 1;
	lower_bound2 = 27;
	upper_bound1 = 1;
	upper_bound2 = 27;

	inRange(src_hsv, Scalar(lower_bound1,100,100), Scalar(lower_bound2,255,255), lower_bound_image);
	inRange(src_hsv, Scalar(upper_bound1,100,100), Scalar(upper_bound2,255,255), upper_bound_image);

	addWeighted(lower_bound_image, 1.0, upper_bound_image, 1.0, 0.0, src_colored);	

	RNG rng(12345);
	Mat output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	//findContours function
	findContours(src_colored, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0,0));

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

	//checking the radii, making sure greater than 100 for these purposes. -- can change
	vector<Point2f> candidates;
	float largest_radius = 0;
	float largest_idx = -1;
	for(size_t i = 0; i < radius.size(); i++){
		if(radius[i] > largest_radius && radius[i] > 3){
			largest_radius = radius[i];
			largest_idx = i;
		}
		//	cout << "center: " << center[i] << "       " << "radius: " << radius[i] << endl;
	}
	//push the largest on
	if(largest_idx != -1){
		cout << " FOUND IT ON FRAME" << endl;
		candidates.push_back(center[largest_idx]);
		outputAngle(src, candidates); 
	}
	else{
		cout << "Nothing detected, Angle to travel : -1" << endl;
	}

	//drawing contours and rectangless and circles -- maybe not totally necessary -- only for the candidates

	Mat drawing = Mat::zeros(src_colored.size(), CV_8UC3);
	for(size_t i = 0; i < contours.size(); i++){
		Scalar color = Scalar(rng.uniform(0,255), rng.uniform(0,255), rng.uniform(0,255));
		drawContours(drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point());
		rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
		circle(drawing, center[i], (int)radius[i], color, 2, 8, 0);
	}

	//	namedWindow("Color Contours", WINDOW_NORMAL);
	//	resizeWindow("Color Contours", 600,600);
	//	imshow("Color Contours", drawing);

	return drawing;
}

void outputAngle(Mat src, vector<Point2f> candidates){
	//using the colored image
	//we have a point, which is the center of a cirlce, should only have 1.
	int midX = src.size().width / 2;
	int midY = src.size().height / 2;
	int x, y, tx, ty, targetx = -1, targety = -1;
	int flag = 0;
	//cout << "SIZE OF CANDIDATES VECTOR: " << candidates.size() << endl;
	for(size_t i = 0; i < candidates.size(); i++){
		if(flag != 0){
			break;
		}
		x = candidates[i].x;
		y = candidates[i].y;
		for(tx = -5; tx < 6; tx++){
			if(flag != 0)	{ break; }
			for(ty = -5; ty < 6; ty++){
				int correctColor = src.at<uchar>(y + ty, x + tx);
				if(correctColor != 0){
					flag = 1;
					targetx = x;
					targety = y;
					break;
				}
			}
		}
	}

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

	int quadrant;
	if(targety < midY && targetx > midX){
		quadrant = 1;
		cout << "Angle to travel : " << angle;
	}
	else if(targety < midY && targetx < midX){
		quadrant = 2;
		cout << "Angle to travel : " << 3.14159 - angle;
	}
	else if(targety > midY && targetx < midX){
		quadrant = 3;
		cout << "Angle to travel : " << 3.14159 + angle;
	}
	else{
		quadrant = 4;
		cout << "Angle to travel : " << 3.14159 + 1.5707 + angle;
	}
	cout <<  "	" << radius << endl;

	return;
}
