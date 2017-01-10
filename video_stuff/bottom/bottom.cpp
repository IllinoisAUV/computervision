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

using namespace std;
using namespace cv;

static void findSquares(const Mat& image, vector<vector<Point> >& squares);
static double angle( Point pt1, Point pt2, Point pt0 );
static void drawSquares( Mat& image, const vector<vector<Point> >& squares );
Mat color(Mat &src);

int numOfIterations=0;

int main(){

	VideoCapture cap("downward_view.mp4");
	if(!cap.isOpened()){
		cout << "Cannot open the video file. \n";
		return -1;
	}
   
	double fps = cap.get(CV_CAP_PROP_FPS); //
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
		numOfIterations++;
		Mat colored_img = color(frame);
		findSquares(colored_img, squares);
		drawSquares(colored_img, squares);
		imshow("colored", colored_img);
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
	lower_bound1 = 0;
	lower_bound2 = 27;
	upper_bound1 = 155;
	upper_bound2 = 170;

	inRange(src_hsv, Scalar(lower_bound1,100,100), Scalar(lower_bound2,255,255), lower_bound_image);
	inRange(src_hsv, Scalar(upper_bound1,100,100), Scalar(upper_bound2,255,255), upper_bound_image);

	addWeighted(lower_bound_image, 1.0, upper_bound_image, 1.0, 0.0, src_colored);	
	return src_colored;

	//thresholding without color	NO GOOD
/*	Mat src_thresh, src_gray;
	int threshold_value = 200, max_BINARY_VALUE = 255, threshold_type = 1;
	cvtColor(src, src_gray, CV_BGR2GRAY);
	threshold(src_gray, src_thresh, threshold_value, max_BINARY_VALUE, threshold_type);
	src_thresh.convertTo(src_thresh, CV_8UC1);
	return src_thresh;
*/
}

static void findSquares( const Mat& image, vector<vector<Point> >& squares )
{
	int N = 11;
	int thresh = 50;
	squares.clear();

    Mat pyr, timg, gray0(image.size(), CV_8U), gray;


    // down-scale and upscale the image to filter out the noise
    pyrDown(image, pyr, Size(image.cols/2, image.rows/2));
    pyrUp(pyr, timg, image.size());
    vector<vector<Point> > contours;

    // find squares in every color plane of the image
    for( int c = 0; c < 3; c++ )
    {
        int ch[] = {c, 0};
       // mixChannels(&timg, 1, &gray0, 1, ch, 1);

        // try several threshold levels
        for( int l = 0; l < N; l++ )
        {
            // hack: use Canny instead of zero threshold level.
            // Canny helps to catch squares with gradient shading
            if( l == 0 )
            {
                // apply Canny. Take the upper threshold from slider
                // and set the lower to 0 (which forces edges merging)
                Canny(image, gray, 0, thresh, 5);
                // dilate canny output to remove potential
                // holes between edge segments
                dilate(gray, gray, Mat(), Point(-1,-1));
            }
            else
            {
                // apply threshold if l!=0:
                //     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
                gray = image >= (l+1)*255/N;
            }

            // find contours and store them all as a list
            findContours(gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
//		cout << "Size of contours: " << contours.size() << endl;
            vector<Point> approx;

            // test each contour
            for( size_t i = 0; i < contours.size(); i++ )
            {
                // approximate contour with accuracy proportional
                // to the contour perimeter
                approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);

                // square contours should have 4 vertices after approximation
                // relatively large area (to filter out noisy contours)
                // and be convex.
                // Note: absolute value of an area is used because
                // area may be positive or negative - in accordance with the
                // contour orientation
                if( approx.size() == 4 &&
                    fabs(contourArea(Mat(approx))) > 1000 &&
                    isContourConvex(Mat(approx)) )
                {
                    double maxCosine = 0;

                    for( int j = 2; j < 5; j++ )
                    {
                        // find the maximum cosine of the angle between joint edges
                        double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
                        maxCosine = MAX(maxCosine, cosine);
                    }

                    // if cosines of all angles are small
                    // (all angles are ~90 degree) then write quandrange
                    // vertices to resultant sequence
                    if( maxCosine < 0.3 )
                        squares.push_back(approx);
                }
            }
        }
    }
}

// helper function:
// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2
static double angle( Point pt1, Point pt2, Point pt0 )
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

static void drawSquares( Mat& image, const vector<vector<Point> >& squares )
{
    Mat image_color;
	cvtColor(image, image_color, CV_GRAY2BGR);
	//cout << "squares size: " << squares.size() << endl;
	for( size_t i = 0; i < squares.size(); i++ ){
        const Point* p = &squares[i][0];
        int n = (int)squares[i].size();
        polylines(image_color, &p, &n, 1, true, Scalar(0,255,0), 3, 8);
	}

/**************************************************************************************/

	//finding the angle to travel

	//try to find the longest contour.
	int longest=0,longest_idx=-1;
	for(size_t i = 0; i < squares.size(); i++){
		if(squares[i].size() > longest){
			longest = squares[i].size();
			longest_idx = i;
		}
	}
	
	if(longest_idx != -1){
		Point start(squares[longest_idx][0].x, squares[longest_idx][0].y);
		Point end(squares[longest_idx][longest - 1].x, squares[longest_idx][longest - 1].y);
	
		double angle = atan2(end.y - start.y, end.x - start.x);
		if(angle < 0) { angle  = angle + 360; }
		cout << "angle: " << angle << endl;;
		line(image_color, start, end, Scalar(0,0,255), 3, 8);
	}

	if(squares.size() > 0){
		cout << "frame count = " << numOfIterations << endl;
		stringstream ss;
		ss << numOfIterations;
		string str = ss.str();
		imwrite("test" + str + ".png" , image_color);
		cout << endl;
	}

	string wndname = "Detected tape";
    imshow(wndname, image_color);

}
/*	//now draw a line connecting these longest points
	if(longest_idx != -1){
		//cout << "before  init" << endl;
	//		cout << squares[longest_idx][0].x << "	" << squares[longest_idx][0].y << endl;
	
		Point start(squares[longest_idx][0].x, squares[longest_idx][0].y);
		Point end(squares[longest_idx][longest - 1].x, squares[longest_idx][longest - 1].y);
		if(start.x >= end.x && start.y >= end.y){
			string dir =  "upleft";
		}
		else if(start.x >= end.x && start.y <= end.y){
			string dir = "downleft";
		}
		else if(start.x <= end.x && start.y >= end.y){
			string dir = "upright";
		}
		else{
			string dir = "downright";
		}

		cout <<"start: " << start << "	end: " << end << endl;
		
		line(image_color, start, end, Scalar(0,0,255), 3, 8);
	}

	//int leg_y = abs(end.y - start.y);
	//int leg_x = abs(end.x- start.x);
	//double length = hypot(leg_x, leg_y);

//	if(length > src.width() / 4){
		//this is the long  side, so find the angle of it
		
//	}
//	else{
		//short side, find the angle and flip it
	
//	}
*/	





