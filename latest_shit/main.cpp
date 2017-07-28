#include "vision.h"

unsigned int frame_count = 0;
/*
int main(){
	VideoCapture cap("test.mp4");
	if(!cap.isOpened()){
		cout << "cannot open video file " << endl;
		return -1;
	}
	Vision v;
	targets_data retvals;
	
	while(true){
		if(!cap.read(v.src)){
			cout <<"cannot open video file" << endl;
			return -1;
		}
		imshow("src", v.src);
		frame_count++;
		cout << frame_count << endl;
		resize(v.src, v.src, Size(640, 480));
		if(frame_count % 1 == 0 && frame_count > 1){
			//look for buoy, gate and wire
			retvals = v.findTargets();
			
			//print data
			cout << endl << "vgate_angle: " << retvals.vgate_angle<< " vgate_radius: " << retvals.vgate_radius << " vgate_area: " << retvals.vgate_area << " buoy_angle: " << retvals.buoy_angle << " buoy_radius: " << retvals.buoy_radius << " buoy_area: " << retvals.buoy_area << " wire_angle: " << retvals.wire_angle << " wire_radius: " << retvals.wire_radius << " wire_area: " << retvals.wire_area << endl;
		}
		if(frame_count > 1){
			imshow("source" , v.src);
		}
	}
	return 0;
}
*/
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
        //imshow("src", v.src);
		cout << frame_count << endl;
        frame_count++;

        if(frame_count % 10 == 0 && frame_count > 1){
			retvals = v.findTargets();
        }
		cout << endl << "vgate_angle: " << retvals.vgate_angle<< " vgate_radius: " << retvals.vgate_radius << " vgate_area: " << retvals.vgate_area << " buoy_angle: " << retvals.buoy_angle << " buoy_radius: " << retvals.buoy_radius << " buoy_area: " << retvals.buoy_area << " wire_angle: " << retvals.wire_angle << " wire_radius: " << retvals.wire_radius << " wire_area: " << retvals.wire_area << endl;


        if(waitKey(30) == 27){ // 'esc' to quit
       		break; 
     	}
    }
    return 0;
}
