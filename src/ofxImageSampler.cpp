//
//  ofxImageSampler.cpp
//  traffic_lights_rtsp
//
//  Created by Jakob Marsico on 3/18/14.
//

#include "ofxImageSampler.h"

ofxImageSampler::ofxImageSampler(){}

//--------------------------------------------------------------
void ofxImageSampler::init(int _ID){
    bSettingPoints = false;
    bRegisteredEvents = false;
    bIsSet = false;
    ofLogVerbose() << "cell initialized";
    ID = _ID;
    
    avgCounter = 0;
    numSamples = 5;
    
    for(int i = 0; i < numSamples; i++)
    {
        average.push_back(0);
    }
}


//--------------------------------------------------------------
void ofxImageSampler::setPointsLinked(ofPoint oldP0, ofPoint oldP1, const ofPixels &_pix, ofPoint _startPoint)
{
    
    pix = _pix;
    startX = _startPoint.x;
    startY = _startPoint.y;
    bSettingPoints = true;
    if(!bRegisteredEvents)
    {
        ofRegisterMouseEvents(this); // this will enable our circle class to listen to the mouse events.
        bRegisteredEvents = true;
    }
   
    p.push_back(oldP0);
    p.push_back(oldP1);
    shape.addVertex(p[0]);
    shape.addVertex(p[1]);
    
    ofLog() << "*************************************************";
    ofLogVerbose() << "setPoints " << ID << " started with two points";
    ofLogVerbose() << "cell[" << ID << "] p[0] is: " << p[0].x << " " << p[0].y;
    ofLogVerbose() << "cell[" << ID << "] p[1] is: " << p[1].x << " " << p[1].y;

}


//--------------------------------------------------------------
void ofxImageSampler::setPoints(const ofPixels &_pix, ofPoint _startPoint){
    pix = _pix;
    startX = _startPoint.x;
    startY = _startPoint.y;
    bSettingPoints = true;
    if(!bRegisteredEvents)
    {
        ofRegisterMouseEvents(this); 
        bRegisteredEvents = true;
    }

}

//--------------------------------------------------------------
void ofxImageSampler::addPoint(){
    
    if(bSettingPoints == true)
    {
        if(p.size() == 0)
        {
            ofPoint temp;
            temp.x = mouseClick.x;
            temp.y = mouseClick.y;
            p.push_back(temp);
            shape.addVertex(p[0]);
        }
        else if(p.size() == 1)
        {
            ofPoint temp;
            temp.x = mouseClick.x;
            temp.y = mouseClick.y;
            p.push_back(temp);
            shape.addVertex(p[1]);
        }
        else if(p.size() == 2)
        {
            ofPoint temp;
            temp.x = mouseClick.x;
            temp.y = mouseClick.y;
            p.push_back(temp);
            shape.addVertex(p[2]);
        }
        else if(p.size() == 3)
        {
            ofPoint temp;
            temp.x = mouseClick.x;
            temp.y = mouseClick.y;
            p.push_back(temp);
            shape.addVertex(p[3]);
            shape.close();
            bSettingPoints = false;
            bIsSet = true;
    
            ofLogVerbose("ofxImageSampler") << "sample cell " << ID << " is set.";
            ofUnregisterMouseEvents(this);
        }
        
        if(bIsSet)
        {
            getPixLocations();
        }
    }
}

//--------------------------------------------------------------
void ofxImageSampler::getPixLocations(){
    
    for(int x = 0; x < pix.getWidth(); x++)
    {
        for(int y = 0; y < pix.getHeight(); y++)
        {
            if(shape.inside(x + startX, y + startY))
            {
                ofPoint index;
                index.set(x+startX, y+startY);
                pixIn.push_back(index);
            }
        }
    }
    
    ofLogVerbose("ofxImageSampler") << "num of pix in sample cell " << ID <<  ": " << pixIn.size();
}


//--------------------------------------------------------------
void ofxImageSampler::update(const ofPixels &_pix){
    pix = _pix;
    int total = 0;
    //if pixIn is not empty
    if(pixIn.size() > 0)
    {
        for(int i = 0; i < pixIn.size(); i++)
        {
            total = total + pix.getColor(pixIn[i].x, pixIn[i].y).getBrightness();
        }
    
        brightness = total / pixIn.size();
        //total = 0;
    }
    setCellColor(pix);
    
}

//--------------------------------------------------------------
void ofxImageSampler::setCellColor(const ofPixels &_pix){
    //ofLog() << "get Cell Color for: " << ID;
    pix = _pix;
    float r = 0;
    float g = 0;
    float b = 0;
    
    if(pixIn.size() > 0)
    {
        for(int i = 0; i < pixIn.size(); i++)
        {
            ofColor tempCol = pix.getColor(pixIn[i].x, pixIn[i].y);
            r = r + tempCol.r;
            g = g + tempCol.g;
            b = b + tempCol.b;
        }
        
        cellColor.set((int)r/pixIn.size(), (int)g/pixIn.size(), (int)b/pixIn.size());

    }
    
}


//--------------------------------------------------------------
bool ofxImageSampler::isPointsSet(){
    return bIsSet;
}

//--------------------------------------------------------------
bool ofxImageSampler::isSettingPoints(){
    return bSettingPoints;
}

//--------------------------------------------------------------
ofColor ofxImageSampler::getCellColor(){
    return cellColor;
}

//--------------------------------------------------------------
int ofxImageSampler::getAverageBrightness(int _numSamples){

    numSamples = _numSamples;

    //remove old reading
    runningTotal = runningTotal - average[avgCounter];
    
    //set that lcoation to newest value
    average[avgCounter] = brightness;
    
    //add the newest reading
    runningTotal += average[avgCounter];
    
    //increment the counter (location in vector)
    avgCounter++;

    if(avgCounter >= average.size()-1)
    {
        avgCounter = 0;
    }
    
    if(numSamples != average.size())
    {
        
        average.clear();
        runningTotal = 0;
        
        //runningTotal = 0;
        while(numSamples > average.size())
        {
            average.push_back(brightness);
        }
        
        while(average.size() > numSamples)
        {
            average.pop_back();
        }
    }
    
    if(runningTotal < 0) runningTotal = 0;
    int avge = (int)(runningTotal / average.size());
    if(avge > 255) avge = 255;


    return avge;
    ofLogVerbose() << "ID: " << ID << avge;
}

//--------------------------------------------------------------
void ofxImageSampler::draw(){
    alpha = 50 ;
    ofFill();
    if(bIsSet){
        ofSetColor(167,160,160, alpha);
        ofBeginShape();
        ofVertex(p[0]);
        ofVertex(p[1]);
        ofVertex(p[2]);
        ofVertex(p[3]);
        ofEndShape();
    }
    
    ofNoFill();
    ofSetColor(0, 255, 0);
    shape.draw();
    ofDrawBitmapString(ofToString(brightness), shape.getCentroid2D().x, shape.getCentroid2D().y);
    
}


//--------------------------------------------------------------
void ofxImageSampler::reset(){
    shape.clear();
    p.clear();
    bIsSet = false;
    ofLogVerbose() << "ID: " << ID << " is RESET";
    bSettingPoints = false;
    pixIn.clear();
}

//--------------------------------------------------------------
void ofxImageSampler::mouseMoved(ofMouseEventArgs & args){}
void ofxImageSampler::mouseDragged(ofMouseEventArgs & args){}
void ofxImageSampler::mousePressed(ofMouseEventArgs & args){
    mouseClick.x = args.x;
    mouseClick.y = args.y;
    addPoint();
}
void ofxImageSampler::mouseReleased(ofMouseEventArgs & args){}

