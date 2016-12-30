#include "detect.h"

Detect::Detect(string imgname){
	//variables -- maybe put this one into a separate helper...
	float h = 3;
	int templateWindowSize = 7;
	int searchWindowSize = 21;
	int thresh = 180;
	int max_thresh = 255;
	RNG rng(12345);
	int threshold_value = 180;		//???
	int threshold_type = 3;                 //0, 1, 3 are the best
	int const max_value = 255;
	int const max_type = 4;
	int const max_BINARY_value = 255;
	src = imread(imgname, 1);		//reading the image
	cvtColor(src, src_gray, CV_BGR2GRAY);	//making src_gray
	fastNlMeansDenoising(src_gray, src_denoised, h, templateWindowSize, searchWindowSize);	//making src_denoised
	threshold(src_denoised, src_thresh, threshold_value, max_BINARY_value, threshold_type);	//making src_thresh
	
	//making src_colored.. this will need to change depending  on what color we want to detect
	Mat src_hsv;
	cvtColor(src, src_hsv, COLOR_BGR2HSV);
	int lower_bound1, lower_bound2;
	int upper_bound1, upper_bound2;
	string colorToDetect;
	Mat lower_bound_image, upper_bound_image;
/*	cout << "What color should we detect?" << endl;
	cout << "Enter orange, red, blue, yellow,  green, ...." << endl; 	//...
	cin >> colorToDetect;
	if(colorToDetect == "orange"){
		lower_bound1 = 0;
		lower_bound2 = 27;
		upper_bound1 = 155;
		upper_bound2 = 170;
	}
	if(colorToDetect == "red"){
		lower_bound1 = 0;
		lower_bound2 = 27;
		upper_bound1 = 155;
		upper_bound2 = 170;
	}
	if(colorToDetect == "blue"){
		lower_bound1 = 70;
		lower_bound2 = 135;
		upper_bound1 = 70;
		upper_bound2 = 135;
	}
	if(colorToDetect == "yellow"){
		lower_bound1 = 18;
		lower_bound2 = 42;
		upper_bound1 = 18;
		upper_bound2 = 42;
	}
	if(colorToDetect == "green"){
		lower_bound1 = 34;
		lower_bound2 = 80;
		upper_bound1 = 34;
		upper_bound2 = 80;
	}
*/	

	//orange
	lower_bound1 = 0;
	lower_bound2 = 27;
	upper_bound1 = 155;
	upper_bound2 = 170;
	

	inRange(src_hsv, Scalar(lower_bound1,100,100), Scalar(lower_bound2,255,255), lower_bound_image);
	inRange(src_hsv, Scalar(upper_bound1,100,100), Scalar(upper_bound2,255,255), upper_bound_image);


	addWeighted(lower_bound_image, 1.0, upper_bound_image, 1.0, 0.0, src_colored);	
}

void Detect::detectBalloon(){
	//do contour detection / rectangles on the colored image.
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
	for(size_t i = 0; i < radius.size(); i++){
		//cout << center[i].x << endl;
		if(radius[i] > 10){
			cout << "center: " << center[i] << endl;
			candidates.push_back(center[i]);
		}
	//	cout << "center: " << center[i] << "       " << "radius: " << radius[i] << endl;
	}

	//now have a vector of possible icons that we want to explore
	//so now we check to see if the color matches by checking the colored picture
	//checking for which direction to output based on the candidates' x,y value
	outputAngle(candidates);

	//drawing contours and rectangless and circles -- maybe not totally necessary -- only for the candidates

	Mat drawing = Mat::zeros(src_colored.size(), CV_8UC3);
	for(size_t i = 0; i < contours.size(); i++){
		Scalar color = Scalar(rng.uniform(0,255), rng.uniform(0,255), rng.uniform(0,255));
		drawContours(drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point());
		rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
		circle(drawing, center[i], (int)radius[i], color, 2, 8, 0);
	}
	
	namedWindow("Color Contours", WINDOW_NORMAL);
	resizeWindow("Color Contours", 600,600);
	imshow("Color Contours", drawing);
	
}

void Detect::outputAngle(vector<Point2f> candidates){
	//using the colored image
	//we have a point, which is the center of a cirlce, should only have 1.
	int midX = src.size().width / 2;
	int midY = src.size().height / 2;
	int x, y, tx, ty, targetx = -1, targety = -1;
	int flag = 0;
	cout << "SIZE: " << candidates.size() << endl;
	for(size_t i = 0; i < candidates.size(); i++){
		if(flag != 0){
			break;
		}
		x = candidates[i].x;
		y = candidates[i].y;
		for(tx = -5; tx < 6; tx++){
			if(flag != 0)	{ break; }
			for(ty = -5; ty < 6; ty++){
				int correctColor = src_colored.at<uchar>(y + ty, x + tx);
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
	
	cout << "target x = " << targetx << " target y = " << targety << endl;
	cout << "midx = " << midX << " midy = " << midY << endl;

	if(targetx == -1 && targety == -1){
		cout << "Nothing detected, Angle to travel : -1" << endl;
		return;
	}	

	double adj = targetx - midX;
	double opp = targety - midY;
	double angle = atan(opp / adj);
	
	int quadrant;
	if(targety < midY && targetx > midX){
		quadrant = 1;
		cout << "Angle to travel : " << -angle << endl;
	}
	else if(targety < midY && targetx < midX){
		quadrant = 2;
		cout << "Angle to travel : " << 3.14159 - angle << endl;
	}
	else if(targety > midY && targetx < midX){
		quadrant = 3;
		cout << "Angle to travel : " << -angle << endl;
	}
	else{
		quadrant = 4;
		cout << "Angle to travel : " << -angle << endl;
	}
	
	return;
}

void Detect::displayImages(){
	string source = "source";
	string gray = "gray";
	string denoised = "denoised";
	string thresh = "thresholded";
	string colored = "colored";

	namedWindow(source, WINDOW_NORMAL);
	resizeWindow(source, 2000, 2000);
	imshow(source, src);
	
	namedWindow(gray, WINDOW_NORMAL);
	resizeWindow(gray, 2000, 2000);
	imshow(gray, src_gray);

	namedWindow(denoised, WINDOW_NORMAL);
	resizeWindow(denoised, 2000, 2000);
	imshow(denoised, src_denoised);
	
	namedWindow(thresh, WINDOW_NORMAL);
	resizeWindow(thresh, 2000, 2000);
	imshow(thresh, src_thresh);
	
	namedWindow(colored, WINDOW_NORMAL);
	resizeWindow(colored, 2000, 2000);
	imshow(colored, src_colored);

}

