#pragma once

#include "ofMain.h"
#include "ofxImageSampler.h"
#include "ofxGui.h"
#include "ofxXmlSettings.h"

class ofApp : public ofBaseApp{

#define NUMSAMPLES 8
    
public:
    void setup();
    void update();
    void draw();
    
    void loadCellsFromXml();
    void saveCellsToXml();
    
    ofxImageSampler cells[NUMSAMPLES];
    
    ofVideoPlayer vid;
    ofImage vidPix;
    ofPoint startPosition;
    vector <int> brightVals;
    
    ofxPanel gui;
    ofxButton bLoadCells;
    ofxButton bSaveCells;
    ofxButton cellReset;
    ofxToggle bReady;
    ofxToggle bLinkCells;
    ofxIntSlider avgAmt;
    ofxToggle bShowColor;

    ofxXmlSettings pointsXML;
		
};
