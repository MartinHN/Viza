//
//  BaseFileLoader.h
//  ViZa
//
//  Created by martin hermant on 07/04/15.
//
//

#ifndef __ViZa__BaseFileLoader__
#define __ViZa__BaseFileLoader__

#include "ofMain.h"
#include "FileUtils.h"
#include "Container.h"
#include "Poco/Task.h"
#include "Poco/Notification.h"
#include "Poco/Random.h"





class BaseFileLoader {
    
public:
    
    typedef map<string,BaseFileLoader*(*)(const string & s)> loaders_map_type;
    
    static BaseFileLoader* getLoader(string const & extension,string const & name);
    static loaders_map_type * getMap();
    static vector<string> getAllowedExtensions();
    vector <string> extensions;
    
    BaseFileLoader(const std::string& name);
    virtual ~BaseFileLoader();
    string name;
    
    //filled only first time as it's a coherent database
    typedef struct {
        
        vector<string> attributeNames;
        
        // contain Viza-added Attribute names : length, start idx, relativeStartidx
        bool hasVizaMeta = false;
        unsigned int totalContainers;
        unsigned int totalSong;

    }GlobalInfo;
    
    typedef struct {
        unsigned int containerIdx=-1;
        unsigned int songIdx = -1;
        string parsedFile = "";
        int numElements=0;
        Container::SongMeta song;
        map<string,vector<float>> data;
        bool isCaching ;
        
    }ContainerBlockInfo;
    
    
    virtual bool getCachedInfo(const string & annotationdir)=0;
    virtual bool fillContainerBlock(const string  annotationpath) = 0;
    virtual vector<string> getAttributeNames(const string & path) = 0;
    virtual bool hasCachedInfo() = 0;
    virtual int cacheInfo() = 0;
    virtual void endCaching(){};

    int SupportedNumThreads = 6;
    ContainerBlockInfo * containerBlock;
    static GlobalInfo globalInfo;
    
    static void linkLoaders();
    
    void runTask();
    
    void runTask(ContainerBlockInfo * c){
        containerBlock  = c;
        this->runTask();
    }
    float progress;
    
    static vector<string> attrSubset;
    static string audioFolderPath;
    static string annotationFolderPath;
    static bool init;

    
    string searchAudiofromAnal(const string & s,const string & audioFolder);


protected:
    
    // return actual number of containerCreated
    virtual int loadFile() = 0;
    
    static  loaders_map_type * loadersMap;
    
    void setSongInfo();
    
};

template<typename T> BaseFileLoader * createT(const string & s) { return new T(s); }




#endif /* defined(__ViZa__BaseFileLoader__) */
