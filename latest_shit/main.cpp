#include "vision.h"

unsigned int frame_count = 0;
int main(){

	VideoCapture cap("test.mp4");
	if(!cap.isOpened()){
		cout << "Cannot open the video file. \n";
		return -1;
	}
   
	Vision v;
	targets_data retvals;
 
	while(1){
        if (!cap.read(v.src)){ // if not success, break loop
        	// read() decodes and captures the next frame.
            cout<<"\n Cannot read the video file. \n";
   		    break;
	    }
        resize(v.src, v.src, Size(480, 640));
		cout << frame_count << endl;
        frame_count++;

        if(frame_count % 10 == 0 && frame_count > 1700){
			retvals = v.findTargets();
            imshow("src", v.src);
        }
		cout << endl << "vgate_angle: " << retvals.vgate_angle<< " vgate_radius: " << retvals.vgate_radius << " vgate_area: " << retvals.vgate_area << " buoy_angle: " << retvals.buoy_angle << " buoy_radius: " << retvals.buoy_radius << " buoy_area: " << retvals.buoy_area << " wire_angle: " << retvals.wire_angle << " wire_radius: " << retvals.wire_radius << " wire_area: " << retvals.wire_area << endl;


        if(waitKey(30) == 27){ // 'esc' to quit
       		break; 
     	}
    }
    return 0;
}
