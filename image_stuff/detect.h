#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "highgui.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace std;
using namespace cv;

//assuming that we will have a main method that calls this for each image from the camera

class Detect{

	public:
		Detect(string imgname);
		void detectBalloon();
		void displayImages();
				
	private:
		Mat src;
		Mat src_gray;
		Mat src_denoised;
		Mat src_thresh;
		Mat src_colored;
		
		void findingContours(int, void*);
		void outputAngle(vector<Point2f> candidates);
};
