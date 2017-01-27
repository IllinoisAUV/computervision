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
            continue;
        }
		numOfIterations++;
		Mat colored_img = color(frame);
		findSquares(colored_img, squares);
		drawSquares(colored_img, squares);
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
	Mat drawn_squares(image.size().height, image.size().width, CV_8UC3, Scalar(0,0,0));
	cvtColor(image, image_color, CV_GRAY2BGR);
	//cvtColor(image, drawn_squares, CV_GRAY2BGR);
	//cout << "squares size: " << squares.size() << endl;
	for( size_t i = 0; i < squares.size(); i++ ){
        	const Point* p = &squares[i][0];
        	int n = (int)squares[i].size();
        	polylines(image_color, &p, &n, 1, true, Scalar(0,255,0), 3, 8);
        	polylines(drawn_squares, &p, &n, 1, true, Scalar(0,255,0), 3, 8);
	}

	//drawn_squares just holds the drawn image
	string blank = "BLANK IMAGE";
    imshow(blank, drawn_squares);

/**************************************************************************************/

	//finding the angle to travel
	//do this by making a bounding rect on the drawn_image, and we will get the endpoints

	RNG rng(12345);	
	Mat another_image;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

//	threshold( drawn_squares, another_image, 100, 255, THRESH_BINARY );
	/// Find contours
	cvtColor(drawn_squares, another_image, CV_BGR2GRAY);
	findContours( another_image, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

	/// Find the rotated rectangles and ellipses for each contour
	vector<RotatedRect> minRect( contours.size() );
	vector<RotatedRect> minEllipse( contours.size() );

	for( int i = 0; i < contours.size(); i++ ){
    	minRect[i] = minAreaRect( Mat(contours[i]) );
        if( contours[i].size() > 5 ){
        	minEllipse[i] = fitEllipse( Mat(contours[i]) );
		}
    }

	/// Draw contours + rotated rects + ellipses
	Mat drawing = Mat::zeros( another_image.size(), CV_8UC3 );
	for( int i = 0; i< contours.size(); i++ ){
    	Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        // contour
        drawContours( drawing, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
        // ellipse
        //ellipse( drawing, minEllipse[i], color, 2, 8 );
        // rotated rectangle
        Point2f rect_points[4]; minRect[i].points( rect_points );
        for( int j = 0; j < 4; j++ ){
          line( drawing, rect_points[j], rect_points[(j+1)%4], color, 1, 8 );
		}
    }

	string another = "another image";
    imshow(another, drawing);

	//have the vertices of the rectangle, now calculate the angle to travel
	//assume only 2 that we have found
	Point2f first[4];
	Point2f second[4];

	if(minRect.size() > 0){
		minRect[0].points(first);
		cout << "ITERATIONS: " << numOfIterations << endl;
		cout << "The angle to travel for the first rectangle is: " << minRect[0].angle << endl;
	}
	if(minRect.size() >= 2){
		minRect[1].points(second);
		cout << "The angle to travel for the second rectangle is: " << minRect[1].angle << endl;
	}



/*
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
		//cout << "angle: " << angle << endl;;
		line(image_color, start, end, Scalar(0,0,255), 3, 8);
	}

	if(squares.size() > 0){
		//cout << "frame count = " << numOfIterations << endl;
		stringstream ss;
		ss << numOfIterations;
		string str = ss.str();
		imwrite("test" + str + ".png" , image_color);
		cout << endl;
	}
*/
	string wndname = "Detected tape";
    imshow(wndname, image_color);

}



