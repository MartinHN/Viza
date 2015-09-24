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
    bool loadPosition();
    bool crawlAnnotations(string annotationPath,string audioPath="");
    
    
    void threadedFunction() override;

    void save();
    
    float progressPct;


    
    void updateGlobalInfo();
    static bool loadAnalysisFiles(string segPath="",string audioPath="");
    bool isCaching ;
    vector<string> extensions ;
    vector<filesystem::path> Paths;
    
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
