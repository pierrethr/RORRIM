#pragma once

#include "ofMain.h"
#include "ofxQTKitVideoGrabber.h"
#include "ofxUVC.h"
#include "ofxSimpleGuiToo.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void mouseMoved(int x, int y );
    
    
        ofxQTKitVideoGrabber      cam;
        //ofVideoGrabber      cam;
        ofxUVC uvcControl;
        
        vector<ofxUVCControl> controls;
        
        ofImage             input;
        ofImage             output;
    
        void getPixelsFrom (int x, int y, int ox, int oy);
        void updateUVCSettings();
    
        void initWithCamera();
        void initWithImage();
		
};
