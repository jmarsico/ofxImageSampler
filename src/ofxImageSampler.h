//
//  ofxImageSampler.h
//  traffic_lights_rtsp
//
//  Created by Jakob Marsico on 3/18/14.

// found on github!
//

#ifndef __traffic_lights_rtsp__sampleCell__
#define __traffic_lights_rtsp__sampleCell__

#include <iostream>
#include "ofMain.h"

class ofxImageSampler{
public:
    ofxImageSampler();
    
    void init(int _ID);
    void setPointsLinked(ofPoint oldP0, ofPoint oldP1, const ofPixels &_pix, ofPoint _startPoint);
    void setPoints(const ofPixels &_pix, ofPoint _startPoint);
    
    void draw();
    void update(const ofPixels &_pix);
    int getAverageBrightness(int _numSamples);
    ofColor getCellColor();
    void reset();
    
    //these functions can be called externally when loading from XML
    bool isPointsSet();
    bool isSettingPoints();
    void addPoint();
    void getPixLocations();
    
    
    vector <ofPoint> p;
    ofPoint mouseClick;
    vector <ofPoint> pixIn;
    ofPolyline shape;
    
    
    int ID;
    
    ofPixels pix;
    int startX;
    int startY;
    bool bSettingPoints;
    
    void mouseMoved(ofMouseEventArgs & args);
    void mouseDragged(ofMouseEventArgs & args);
    void mousePressed(ofMouseEventArgs & args);
    void mouseReleased(ofMouseEventArgs & args);
    bool bIsSet;
    
    
protected:
    
    void setCellColor(const ofPixels &_pix);
    
    bool bRegisteredEvents;
    
    int increment;
    int brightness;
    int alpha;
    bool secondSet;
    ofColor cellColor;
    
    vector <int> average;
    int avgCounter;
    int runningTotal;
    int numSamples;
    
    
};



#endif /* defined(__traffic_lights_rtsp__sampleCell__) */
