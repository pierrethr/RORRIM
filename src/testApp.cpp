#include "testApp.h"

int cols = 30;
int rows = 15;
float tileW, tileH;
int xOffsetMax = 150;
int yOffsetMax = 50;

float exposure, brightness, contrast, whitebalance, gain, saturation, sharpness = 1;
bool bUpdateUVCSettings = false;

unsigned char *inputPixels;
unsigned char *outPixels;

bool bDebug = false;

string imgFile = "";

//--------------------------------------------------------------
void testApp::setup(){
    ofSetVerticalSync( true );
    ofSetFrameRate(60);
    
    if (imgFile == "") {
        initWithCamera();
    } else {
        initWithImage();
    }
    
    
    //------------------------------
    gui.addTitle("Camera controls");
    gui.addSlider("Exposure", exposure, 0, 1);
    gui.addSlider("Brightness", brightness, 0, 1);
    gui.addSlider("Contrast", contrast, 0, 1);
    gui.addSlider("White balance", whitebalance, 0, 1);
    gui.addSlider("Gain", gain, 0, 1);
    gui.addSlider("Saturation", saturation, 0, 1);
    gui.addSlider("Sharpness", sharpness, 0, 1);
    gui.addButton("UPDATE CAMERA SETTINGS", bUpdateUVCSettings);
    
    gui.loadFromXML();
    //gui.toggleDraw();
    
}


//--------------------------------------------------------------
void testApp::initWithCamera () {
    cam.setVerbose(true);
    //cam.listDevices();
    //cam.setDeviceID(0);
    cam.setDesiredFrameRate(60);
    cam.initGrabber(640,480);
    
    //uvcControl.useCamera(0x5ac,0x8507, 0x00);// Built-in iSight
    uvcControl.useCamera(0x46d,0x991, 0x00); // Logitech C905
    
    uvcControl.setAutoExposure(false);
    uvcControl.setAutoWhiteBalance(false);
    uvcControl.setAutoFocus(true);    
    
    input.allocate (cam.getWidth(), cam.getHeight(), OF_IMAGE_COLOR);
    output.allocate(cam.getWidth(), cam.getHeight(), OF_IMAGE_COLOR);
}


//--------------------------------------------------------------
void testApp::initWithImage () {
    input.allocate(ofGetWidth(),ofGetHeight(),OF_IMAGE_COLOR);
    input.loadImage("RORRIM_LOGO_01.png");
    
    output.allocate(input.getWidth(), input.getHeight(), OF_IMAGE_COLOR);
}

//--------------------------------------------------------------
void testApp::update(){
    ofBackground(0);
    
    if(imgFile == "") cam.update();
    if (bUpdateUVCSettings) updateUVCSettings();
    //cam.grabFrame();
    
    if (cam.isFrameNew() && imgFile == "") {      
        input.setFromPixels(cam.getPixels(), input.getWidth(), input.getHeight(), OF_IMAGE_COLOR);
        input.mirror(false, true);
    }
    
    
    if (input.isAllocated()) {
        if (imgFile == "") {
            tileW = cam.getWidth()/cols;
            tileH = cam.getHeight()/rows;
        } else {
            tileW = input.getWidth()/cols;
            tileH = input.getHeight()/rows;
        }
        
        inputPixels     = input.getPixels();
        outPixels       = output.getPixels();
        
        int x, y;
        int ox, oy;
        int offsetX, offsetY = 0;
        
        for (int i = 0; i < rows*cols; i++) {
            x = ox = i % cols * tileW;
            y = oy = i / cols * tileH;
            
            //Each tile
            if (i%cols != 0) {
                offsetX = ofMap(i%((cols*rows)/rows), 0, cols, xOffsetMax, xOffsetMax*-1);
            }
            
            // Each row
            if (i%cols == 0) {
                offsetX = ofMap(i%((cols*rows)/rows), 0, cols, xOffsetMax, xOffsetMax*-1);
                offsetY = ofMap(i/cols, 0, rows, yOffsetMax, yOffsetMax*-1);
            }
            
            x += offsetX;
            y += offsetY;
            
            if (x < 0) x = 0;
            if (x >= input.getWidth()) x = input.getWidth()-tileW;
            if (y < 0) y = 0;
            if (y >= input.getHeight()) y = input.getHeight()-tileH;
            
            getPixelsFrom(x, y, ox, oy);
        }
        
        output.setFromPixels(outPixels, output.getWidth(), output.getHeight(), OF_IMAGE_COLOR);
    }
}


//--------------------------------------------------------------
void testApp::updateUVCSettings () {
    uvcControl.setExposure(exposure);
    uvcControl.setBrightness(brightness);
    uvcControl.setContrast(contrast);
    uvcControl.setWhiteBalance(whitebalance);
    uvcControl.setGain(gain);
    uvcControl.setSaturation(saturation);
    uvcControl.setSharpness(sharpness);
    
    bUpdateUVCSettings = false;
}

//--------------------------------------------------------------
void testApp::getPixelsFrom (int x, int y, int ox, int oy) {
    for (int i = 0; i < tileH; i++) {
        for (int j = 0; j < tileW; j++) {
            int index = (y+i)*input.getWidth()*3 + (x+j)*3;
            int outIndex = (oy+i)*output.getWidth()*3 + (ox+j)*3;
            
            if (outIndex+2 > output.getWidth()*output.getHeight()*3) outIndex = output.getWidth()*output.getHeight()*3-3;
            if (index+2 > input.getWidth()*input.getHeight()*3) index = input.getWidth()*input.getHeight()*3-3;
            
            outPixels[outIndex] = inputPixels[index];
            outPixels[outIndex+1] = inputPixels[index+1];
            outPixels[outIndex+2] = inputPixels[index+2];
        }
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    output.draw(0,0,ofGetWidth(), ofGetHeight());
    
    if (bDebug) {
        ofSetColor(255, 0, 0);
        for (int i = 0; i < rows*cols; i++) {
            
            int x = (i % cols * tileW) + ((i%cols) * (ofGetWidth() - (cols*tileW)) / (cols));
            int y = (i / cols * tileH) + ((i/cols) * (ofGetHeight() - (rows*tileH)) / (rows));
            
            if (i%cols == 0) {
                ofSetColor(0, 255, 0);
            } else {
                ofSetColor(255, 0, 0);
            }

            
            ofRect(x, y, 2, 2);
        }
        ofSetColor(255, 255, 255);
        
        glPushMatrix();    
        glTranslatef( ofGetWidth(), 0, 0 );
        glRotatef( 90 , 0, 0, 1);
         
        
        ofSetHexColor(0xffffff);
        char reportStr[1024];
        ofDrawBitmapString("CONTROLS\n--------------\nD for debug\nLEFT & RIGHT arrows to add/remove columns\nUP & DOWN arrows to add/remove rows\nMouse X controls X offset\nMouse Y controls Y offset", 20, 20);
        
        ofDrawBitmapString("--------------\nCOLS " + ofToString(cols) + " x " + ofToString(rows) + " ROWS\nX OFFSET " + ofToString(xOffsetMax) + " x " + ofToString(yOffsetMax) + " Y OFFSET", 20, 120);
        
        glPopMatrix();  
    }
    
    gui.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch (key) {
        case OF_KEY_UP:
            rows++;
            break;
        case OF_KEY_DOWN:
            if (rows > 1) rows--;
            break;
        case OF_KEY_RIGHT:
            cols++;
            break;
        case OF_KEY_LEFT:
            if (cols > 1) cols--;
            break;
        case 'd':
            bDebug = !bDebug;
            gui.toggleDraw();
            break;
            
        case 'u':
            bUpdateUVCSettings = true;
            break;
            
        default:
            break;
    }
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
    if (!bDebug) {
        float pX    = (float)ofGetMouseX()/ofGetWidth();
        float pY    = (float)ofGetMouseY()/ofGetHeight();
        
        xOffsetMax  = (int)ofMap(pX, 0, 1, 0, 1000);
        yOffsetMax  = (int)ofMap(pY, 0, 1, 0, 1000);
    }
}
