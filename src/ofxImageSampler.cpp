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
    fillColor.set(200);
    bClick = false;
    
    ofColor c0;
    c0.set(255, 40, 50);
    ofColor c1;
    c1.set(0, 178, 209);
    ofColor c2;
    c2.set(10,10,50);
    
//    gradient.addColor(c0);
    gradient.addColor(c1);
    gradient.addColor(c2);
}

void ofxImageSampler::setFillColor(ofColor col){
    fillColor = col;
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
    
    cellPoint temp0;
    temp0.point = oldP0;
    temp0.bMoving = false;

    cellPoint temp1;
    temp1.point = oldP1;
    temp1.bMoving = false;
    
    
    p.push_back(temp0);
    p.push_back(temp1);
    shape.addVertex(p[0].point);
    shape.addVertex(p[1].point);
    
    ofLog() << "*************************************************";
    ofLogVerbose() << "setPoints " << ID << " started with two points";
    ofLogVerbose() << "cell[" << ID << "] p[0] is: " << p[0].point.x << " " << p[0].point.y;
    ofLogVerbose() << "cell[" << ID << "] p[1] is: " << p[1].point.x << " " << p[1].point.y;

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
            cellPoint temp;
            temp.point = mouseClick;
            temp.bMoving = false;
            p.push_back(temp);
            shape.addVertex(temp.point);
        } else {
        
            if(mouseClick.x < p[0].point.x + 5 &&
               mouseClick.x > p[0].point.x - 5 &&
               mouseClick.y < p[0].point.y + 5 &&
               mouseClick.y > p[0].point.y - 5)
            {
                cellPoint temp;
                temp.point = mouseClick;
                temp.bMoving = false;
                p.push_back(temp);
                shape.addVertex(temp.point);
                shape.close();
                bSettingPoints = false;
                bIsSet = true;
                
                ofLogVerbose("ofxImageSampler") << "sample cell " << ID << " is set.";
            } else {
                cellPoint temp;
                temp.point = mouseClick;
                temp.bMoving = false;
                p.push_back(temp);
                shape.addVertex(temp.point);
            }
        }
        

        
        if(bIsSet)
        {
            getPixLocations();
        }
    }
}

//--------------------------------------------------------------
void ofxImageSampler::getPixLocations(){
    
    pixIn.clear();
    
    for(int x = 0; x < pix.getWidth(); x++)
    {
        for(int y = 0; y < pix.getHeight(); y++)
        {
            if(shape.inside(x + startX, y + startY))
            {
                ofPoint index;
                index.set(x, y);
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
    
    centroid = shape.getCentroid2D();
    
    
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
    ofFill();
    if(bIsSet){
//        fillColor.set( gradient.getColorAtPercent(ofMap(brightness, 0.0, 255.0, 0.0, 1.0)));
        fillColor.set( gradient.getColorAtPercent(0.));

        fillColor.a = ofMap(brightness, 0.0, 255.0, 0.0, 200.0);
        ofSetColor(fillColor);
        ofBeginShape();
        for(int i = 0; i < p.size(); i++)
        {
            ofVertex(p[i].point);
        }
        
        ofEndShape();
    }
    
    for(int i = 0; i < p.size(); i++)
    {
        ofSetColor(255, 50, 100, 100);
        ofCircle(p[i].point, 2);
        
    }
    if(p.size() > 0)
    {
        ofSetColor(50, 50, 255, 100);
        ofCircle(p[0].point, 2);
    }
    
    
    ofNoFill();
    ofSetColor(200, 200, 255);
    shape.draw();
    ofSetColor(0, 255, 0);
//    ofCircle(shape.getCentroid2D().x, shape.getCentroid2D().y, 10);
//    ofDrawBitmapString(ofToString(brightness), shape.getCentroid2D().x, shape.getCentroid2D().y);
    
    
    
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


void ofxImageSampler::movePoint(){
    
    
    for(int i = 0; i < p.size(); i++)
    {
        if(mouseClick.x < p[i].point.x + 5 &&
           mouseClick.x > p[i].point.x - 5 &&
           mouseClick.y < p[i].point.y + 5 &&
           mouseClick.y > p[i].point.y - 5)
        {
            p[i].bMoving = true;
            ofLog() << "POINT GRAB!";
        }
        
    }
    

}

//--------------------------------------------------------------
void ofxImageSampler::mouseMoved(ofMouseEventArgs & args){
    mouseLoc.x = args.x;
    mouseLoc.y = args.y;

}
void ofxImageSampler::mouseDragged(ofMouseEventArgs & args){
    for(int i = 0; i < p.size(); i++)
    {
        if(p[i].bMoving)
        {
            p[i].point.set(args.x, args.y);
            shape.getVertices()[i].set(args.x, args.y);
            
        }
    }


}
void ofxImageSampler::mousePressed(ofMouseEventArgs & args){
    mouseClick.x = args.x;
    mouseClick.y = args.y;
    addPoint();
    movePoint();

    

}
void ofxImageSampler::mouseReleased(ofMouseEventArgs & args){
    //if the mouse is released, stop all points from moving
    
    for(int i = 0; i < p.size(); i++)
    {
        if(p[i].bMoving) getPixLocations();
        p[i].bMoving = false;
        
    }
}

