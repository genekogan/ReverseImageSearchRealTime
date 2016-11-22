#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxJSON.h"
#include "ofxCcv.h"
#include "ofxKDTree.h"
#include "ofxPCA.h"

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();

    void buildKDTree(string folder);
    void getImagePathsRecursive(ofDirectory dir);
    
    void save(string jsonPath);
    void load(string jsonPath);

    
    //void save2(string jsonPath);
    //void load2(string jsonPath);

    ofxKDTree kdTree;

    vector<string> filePaths;

    ofxCcv ccv;

    ofVideoGrabber cam;
    ofImage lastCam;
    int numResults;

    vector<size_t> indexes;
    vector<double> distances;
    
    vector<ofImage> nearestImages;
    
    vector<ofFile> candidateFiles;
    
    
    
    // make a ready flag
    bool ready;
    
    
    
    vector<vector<float> > samples;
    ofxPCA pca;
    
    
    
    
//    ofImage img;
};
