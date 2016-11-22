#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    

    
    numResults = 3;

    cam.initGrabber(640, 480);
    ccv.setup(ofToDataPath("image-net-2012.sqlite3"));
    nearestImages.resize(numResults);

    // build kd-tree from folder
    string folder = "/Users/gene/Media/ImageSets/256_ObjectCategories";//"/Users/gene/Media/ImageSets/small_256"; //"/Users/gene/Media/ImageSets/";
    buildKDTree(folder);
    save(ofToDataPath("results.json"));
    
    // or load from previous model
    //load(ofToDataPath("results.json"));

    
    //    img.load("/Users/gene/Media/ImageSets/small_256/221.tomato/221_0004.jpg");
}

//--------------------------------------------------------------
void ofApp::update(){
    cam.update();
    if (cam.isFrameNew()) {
        lastCam.setFromPixels(cam.getPixels());
        vector<float> encoding = ccv.encode(lastCam, ccv.numLayers()-1);

        vector<float> encoding_pca = pca.transform(encoding);
        
        //vector<float> encoding = ccv.encode(img, ccv.numLayers()-1);
        
//        cout << ofToString(encoding) << endl;
        
        //kdTree.getKNN((vector<double> &) encoding, numResults, indexes, distances);
        kdTree.getKNN((vector<double> &) encoding_pca, numResults, indexes, distances);
        
        for (int i=0; i<numResults; i++) {
            nearestImages[i].load(filePaths[indexes[i]]);
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    if (!ccv.isLoaded()) {
        ofDrawBitmapString("Network file not found!\nCheck your data folder to make sure it exists.", 20, 20);
        return;
    }
    
    lastCam.draw(10, 10);
    for (int i=0; i<3; i++) {
        nearestImages[i].draw(10 + 400*i, 350);
    }
}

//--------------------------------------------------------------
void ofApp::getImagePathsRecursive(ofDirectory dir){
    const string allowed_ext[] = {"jpg", "png", "gif", "jpeg"};
    ofDirectory new_dir;
    int size = dir.listDir();
    for (int i = 0; i < size; i++){
        if (dir.getFile(i).isDirectory()){
            new_dir = ofDirectory(dir.getFile(i).getAbsolutePath());
            new_dir.listDir();
            new_dir.sort();
            getImagePathsRecursive(new_dir);
        }
        else if (find(begin(allowed_ext), end(allowed_ext), ofToLower(dir.getFile(i).getExtension())) != end(allowed_ext)) {
            candidateFiles.push_back(dir.getFile(i));
        }
    }
}

//--------------------------------------------------------------
void ofApp::buildKDTree(string folder) {
    ofLog() << "Gathering images recursively from "+folder;
    ofDirectory dir = ofDirectory(folder);
    getImagePathsRecursive(dir);
    if (candidateFiles.size() == 0) {
        ofLog(OF_LOG_ERROR, "No images found in directory");
        ofExit();
    }
    ofImage image;
    vector<float> encoding;
    for(int i=0; i<candidateFiles.size(); i++) {
        if (i % 200 == 0) ofLog() << " - loading image "<<i<<"/"<<candidateFiles.size();
        
        
        if (i % 4 == 0) {
            image.load(candidateFiles[i]);
            
            
            encoding = ccv.encode(image, ccv.numLayers()-1);
            
            
            
            int nn = encoding.size();
            if (nn != 4096) {
                cout << " encoding " << i << " "<< candidateFiles[i].getAbsolutePath() << " : " << nn << " elements" <<endl;
            }
            bool falseNum = false;
            for (int j=0; j<encoding.size(); j++) {
                if (encoding[j] == NAN || isnan(encoding[j])) {
                    cout << "not a number at " << i << " " << j << endl;
                    falseNum = true;
                }
            }

            
            
            samples.push_back(encoding);
            filePaths.push_back(candidateFiles[i].getAbsolutePath());
        }
    }
    
    
    
//    kdTree.addPoint((vector<double> &) encoding);
//    filePaths.push_back(candidateFiles[i].getAbsolutePath());

    cout << "CALCULATE! " << endl;
    pca.calculate(samples, 100);
    cout << "DONE!" << endl;
    for (int i=0;i<samples.size(); i++) {
        vector<float> sample_pca = pca.transform(samples[i]);
        cout << "size pca " << i << " : " << sample_pca.size() << endl;
        kdTree.addPoint((vector<double> &) sample_pca);
    }
    
    
    kdTree.constructKDTree();
}

//--------------------------------------------------------------
void ofApp::save(string jsonPath) {
    string indexPath = ofToDataPath("index.bin");
    vector<vector<double> > & samples = kdTree.getSamples();
    int numSamples = samples.size();
    if (numSamples != filePaths.size()) {
        ofLog(OF_LOG_ERROR, "number of sampels not matching number of file paths");
        return;
    }
    ofstream file(jsonPath.c_str(), std::ofstream::out | std::ofstream::trunc); // first erase the file
    file.close();
    Json::FastWriter writer;
    for (int k=0; k<numSamples; k++) {
        ofstream file(jsonPath.c_str(), ios_base::out | ios_base::app);
        Json::Value value;
        Json::Value features;
        for (int i=0; i<samples[k].size(); i++) {
            features.append(samples[k][i]);
        }
        value["path"] = filePaths[k];
        value["features"] = features;
        file << writer.write(value);
        file.close();
    }
    kdTree.save(indexPath);
    
    
    
    
    
    
    //
    //
    //
    //ofExit();
}


//--------------------------------------------------------------
void ofApp::load(string jsonPath) {
    ifstream file(jsonPath.c_str());
    Json::Reader reader;
    Json::Value value;
    string line;
    while (getline(file, line)) {
        if (reader.parse(line, value)) {
            Json::Value path = value["path"];
            Json::Value features = value["features"];;
            vector<double> sample;
            for (int i=0; i<features.size(); i++) {
                sample.push_back(features[i].asDouble());
            }
            kdTree.addPoint(sample);
            filePaths.push_back(path.asString());
        }
    }
    string indexPath = ofToDataPath("index.bin");
    kdTree.load(indexPath);
}







/*
void ofApp::save2(string jsonPath) {
    string indexPath = ofToDataPath("index.bin");
    vector<vector<double> > & samples = kdTree.getSamples();
    int numSamples = samples.size();
    if (numSamples != filePaths.size()) {
        ofLog(OF_LOG_ERROR, "number of sampels not matching number of file paths");
        return;
    }
    Json::Value meta;
    Json::Value data;
    for (int k=0; k<numSamples; k++) {
        Json::Value value;
        for (int i=0; i<samples[k].size(); i++) {
            value.append(samples[k][i]);
        }
        meta.append(filePaths[k]);
        data.append(value);
    }
    ofxJSON json;
    json["indexPath"] = indexPath;
    json["meta"] = meta;
    json["data"] = data;
    kdTree.save(indexPath);
    json.save(jsonPath);
    json.clear();
}

//--------------------------------------------------------------
void ofApp::load2(string jsonPath) {
    kdTree.clear();
    ofxJSON json;
    json.open(jsonPath);
    string indexPath = json["indexPath"].asString();
    Json::Value meta = json["meta"];
    Json::Value data = json["data"];
    int nSamples = meta.size();
    if (nSamples != data.size()) {
        ofLog(OF_LOG_ERROR, "number of sampels not matching number of file paths in JSON file");
        return;
    }
    for (int i=0; i<nSamples; i++) {
        vector<double> sample;
        for (int j=0; j<data[i].size(); j++) {
            sample.push_back(data[i][j].asDouble());
        }
        kdTree.addPoint(sample);
        filePaths.push_back(meta[i].asString());
    }
    kdTree.load(indexPath);
}
*/