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


#undef TYPE_BOOL
#include "VizaImpl.pb.h"
#include "VizaGlobal.pb.h"



class FileImporter :public ofThread{
public:
    
    FileImporter();
    //singleton
    static FileImporter* i();
    
    bool hasLoaded=true;

    int numBest=30;
    
    bool savePosition();

    void crawlAnnotations(string annotationPath,string audioPath="");
    
    
    void threadedFunction() override;

    void saveProto();
    
    float progressPct;

    void onTaskQueued(const ofx::TaskQueueEventArgs& args);
    void onTaskStarted(const ofx::TaskQueueEventArgs& args);
    void onTaskCancelled(const ofx::TaskQueueEventArgs& args);
    void onTaskFinished(const ofx::TaskQueueEventArgs& args);
    void onTaskFailed(const ofx::TaskFailedEventArgs& args);
    void onTaskProgress(const ofx::TaskProgressEventArgs& args);
    
    
    static void loadAnalysisFiles(string segPath="",string audioPath="");
    
    
private:
    static FileImporter* instance;
    BaseFileLoader * curLoader;

    
    string annotationfolderPath;
    string audiofolderPath;
    string curAnnotationPath;
    
    void preCache(const vector<filesystem::path>  & annotation);

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

    ofx::TaskQueue queue;
    
    
};


#endif /* defined(__ViZa__FileImporter__) */
