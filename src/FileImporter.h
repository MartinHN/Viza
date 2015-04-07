//
//  FileImporter.h
//  ViZa
//
//  Created by martin hermant on 23/01/15.
//
//

#ifndef __ViZa__FileImporter__
#define __ViZa__FileImporter__


#include "ofMain.h"
#include "BaseFileLoader.h"
#include "ofxTaskQueue.h"

class FileImporter{
public:
    
    FileImporter();
    
    
    int globalCount;
    int numBest;
    ofx::TaskQueue queue;
    bool savePosition();
    
    
    void progress(const ofx::TaskQueueEventArgs& args);
    void crawlAnnotations(string annotationPath,string audioPath="");
    
    void onCompletion();
    int numContainers;
    static FileImporter* i();
    int numDone,totalNumFile;
    float progressPct;
    bool hasLoaded=true;
    
private:
    static FileImporter* instance;
    int sliceCacheSize=2;
    int attributeNamesSize = 50;
    
    void preCache(const vector<ofFile>  & annotation);
    void getSubset(string metaPath);
    
};


#endif /* defined(__ViZa__FileImporter__) */
