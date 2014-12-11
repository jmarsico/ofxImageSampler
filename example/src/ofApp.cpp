#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0);
    ofEnableAlphaBlending();
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetFrameRate(30);
    
    ////////set up the gui
    gui.setup();
    gui.add(bLoadCells.setup("Load Cells"));
    gui.add(bSaveCells.setup("Save Cells"));
    gui.add(cellReset.setup("Clear Cells"));
    gui.add(bReady.setup("Ready for Cells", false));
    gui.add(bLinkCells.setup("Link Cells", true));
    gui.add(avgAmt.setup("Smoothing", 5, 1, 100));
    gui.add(bShowColor.setup("Color or B&W", true));
    gui.setPosition(700, 0);
    gui.loadFromFile("settings.xml");
    
    
    ////////initialize the brightness values
    for(int i = 0; i < NUMSAMPLES; i++)
    {
        brightVals.push_back(0);
    }
    ofLogVerbose() << "size of brightVals array: " << brightVals.size();
    
    
    vid.loadMovie("redcar.mp4");
    vid.play();
    vid.setFrame(2);
    
    //////////initialize all the cells
    for(int i = 0; i < NUMSAMPLES; i++)
    {
        cells[i].init(i);
    }
    
    ////////set up XML for points access
    if(pointsXML.load("cellPoints.xml")) ofLog() << "XML loaded successfully";
    else ofLog() << "XML did not load, check data/ folder";
    
    //
    startPosition.set(0,0);
}


//--------------------------------------------------------------
void ofApp::update(){
    
    vid.update();
    
    if(vid.isFrameNew() && vid.getWidth() > 200)
    {
        vidPix = vid.getPixelsRef();
        vidPix.resize(vid.getWidth(), vid.getHeight());
    }
    else if (vid.getWidth() < 200)
    {
        ofSetColor(255);
        ofDrawBitmapString("no data", 10,10);
    }
    
    //----------------- setting up sampling cells -------------------------
    if(bReady)
    {
        //we use different function for setting the first cell (in case we want to link)
        if(!cells[0].isPointsSet() && !cells[0].isSettingPoints())
        {
            cells[0].setPoints(vidPix, startPosition);
        }
        
        //if the first cell is set, use the second setup function
        if(cells[0].isPointsSet())
        {
            //go through each cell, starting with the second
            for(int i = 1; i < NUMSAMPLES; i++)
            {
                //if the previous cell is set.
                //if this cell is not in the process of setting points
                //if this cell is not already set up
                if(cells[i-1].isPointsSet() && !cells[i].isSettingPoints() && !cells[i].isPointsSet())
                {
                    //set the first two points of this new cell to the last two points of previous cell
                    if(bLinkCells)
                    {
                        cells[i].setPointsLinked(cells[i-1].p[2], cells[i-1].p[3], vidPix, startPosition);
                    }
                    else
                    {
                        cells[i].setPoints(vidPix, startPosition);
                    }
                }
            }
        }
    }
    
    int averageAmount = avgAmt;
    
    //if a cell is set, go ahead and start getting its brightness or color
    for(int i = 0; i < NUMSAMPLES; i++)
    {
        if(cells[i].isPointsSet()){
            
            cells[i].update(vidPix);
            brightVals[i] = cells[i].getAverageBrightness(averageAmount);
            
        }
    }
    
    //save or load cells
    if(bSaveCells) saveCellsToXml();
    if(bLoadCells) loadCellsFromXml();
    
    //reset and re-init cells
    if(cellReset)
    {
        for(int i = 0; i < NUMSAMPLES; i ++)
        {
            cells[i].reset();
            cells[i].init(i);
        }
        bReady = false;
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
  
    gui.draw();
    
    ofSetColor(255);
    vidPix.draw(startPosition);
    
    //draw the cells
    for(int i = 0; i < NUMSAMPLES; i++)
    {
        cells[i].draw();
    }
    
    ofSetColor(0);
    //draw the colors
    int boxSize = ofGetWidth() / (NUMSAMPLES + 1);
    ofPushMatrix();
        ofTranslate(boxSize/2, ofGetHeight()-boxSize - 20);
        for(int i = 0; i < NUMSAMPLES; i++)
        {
            ofPushMatrix();
                ofTranslate(boxSize*i, 0);
                ofFill();
                
                if(bShowColor)
                {
                    ofSetColor(cells[i].getCellColor());
                }
                else
                {
                    ofSetColor(brightVals[i]);
                }
                ofRect(0,0,boxSize, boxSize);
                ofSetColor(255);
                ofDrawBitmapString(ofToString(i), 0,0);
            ofPopMatrix();
        }
    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::loadCellsFromXml(){
    
    ofLog() << "Starting to Load cells from XML...";
    
    for(int i = 0; i < NUMSAMPLES; i++)
    {
        pointsXML.pushTag("CELL", i);
        cells[i].setPoints(vidPix, startPosition);
        
        for(int j = 0; j < 4; j++)
        {
            
            int x = pointsXML.getValue("PT:X", 0, j);
            int y = pointsXML.getValue("PT:Y", 0, j);
            
            //set it to the points of that cell and add to the polyline
            cells[i].mouseClick.x = x;
            cells[i].mouseClick.y = y;
            cells[i].addPoint();
            ofLogVerbose() << "loadFromXML: cell[" << i << "]: point.x = " << x;
            ofLogVerbose() << "loadFromXML: cell[" << i << "]: point.y = " << y;
        }
        
        cells[i].bSettingPoints = false;
        cells[i].bIsSet = true;
        cells[i].getPixLocations();
        pointsXML.popTag();
    }
}

//--------------------------------------------------------------
void ofApp::saveCellsToXml(){
    pointsXML.clear();
    
    for(int i = 0; i < NUMSAMPLES; i++)
    {
        if(cells[i].isPointsSet())
        {
            //create a new cell child
            pointsXML.addTag("CELL");
            //set location to that cell child
            pointsXML.pushTag("CELL", i);
            
            //go through points
            for(int j = 0; j < cells[i].p.size(); j++)
            {
                pointsXML.addTag("PT");
                pointsXML.pushTag("PT", j);
                pointsXML.setValue("X", cells[i].p[j].x);
                pointsXML.setValue("Y", cells[i].p[j].y);
                pointsXML.popTag();
            }
            pointsXML.popTag();
        }
    }
    pointsXML.save("cellPoints.xml");
}


