#include "detect.h"

int main(){
	string img0 = "all_bouy.png";
//	string img2 = "yellow_bouy.png";
	string img1 = "image1.png";
	string img2 = "image2.png";
	string img3 = "image3.png";
	string img4 = "image4.png";
	string img5 = "image5.png";
	string img6 = "image6.png";

	Detect a(img0);

	a.displayImages();
	a.detectBalloon();
	waitKey();
	return 0;
}
