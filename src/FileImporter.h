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

#ifdef PROTOBUF_SUPPORT
#undef TYPE_BOOL
#include "VizaImpl.pb.h"
#include "VizaGlobal.pb.h"
#endif

#include "threadpool.hpp"

class FileImporter :public ofThread{
public:
    
    FileImporter();
    //singleton
    static FileImporter* i();
    
    bool hasLoaded=true;

    int numBest=30;
    
    bool savePosition();

    bool crawlAnnotations(string annotationPath,string audioPath="");
    
    
    void threadedFunction() override;

    void save();
    
    float progressPct;


    
    
    static bool loadAnalysisFiles(string segPath="",string audioPath="");
    bool isCaching ;
    
private:
    static FileImporter* instance;
    BaseFileLoader * curLoader;
    vector<BaseFileLoader::ContainerBlockInfo *>  infos;
    
    string annotationfolderPath;
    string audiofolderPath;
    string curAnnotationPath;
    
    void preCache( );

    void getSubset(string metaPath);
    string findAudioPath(const string & annotationpath);
    
    typedef std::map<string, float> TaskProgress;
    TaskProgress taskProgress;
    void updateProgress();
    void onCompletion();
    

    int numSong;
    int numDone;
    
    int totalNumFile;
    
    
    
    float dbgTime;
    
    typedef ofEventArgs importEventArg;
    void eventRecieved(importEventArg & a);
    ofEvent<importEventArg> importEvent;
    
    void update(ofEventArgs & a);
    bool needUpdate;
    

    
    
};


#endif /* defined(__ViZa__FileImporter__) */
