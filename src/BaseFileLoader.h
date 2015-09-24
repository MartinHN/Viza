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
//#include "Poco/Task.h"
//#include "Poco/Notification.h"
//#include "Poco/Random.h"





class BaseFileLoader {
    
public:
    
    typedef map<string,BaseFileLoader*(*)(const string & s,bool isCaching)> loaders_map_type;
    
    static BaseFileLoader* getLoader(string const & extension,string const & name);
    static loaders_map_type * getMap();
    static vector<string> getAllowedExtensions();
    vector <string> extensions;
    
    BaseFileLoader(const std::string& name,bool isCaching);
    virtual ~BaseFileLoader();
    string name;
    bool isCaching;
    
    //filled only first time as it's a coherent database
    typedef struct {
        
        vector<string> attributeNames;
        
        // contain Viza-added Attribute names : length, start idx, relativeStartidx
        bool hasVizaMeta = false;
        unsigned int totalContainers;
        unsigned int totalSong;
        vector<unsigned int> containerSizes;
        
        void clear(){
            hasVizaMeta = false;
            totalContainers = 0;
            totalSong = 0;
            containerSizes.clear();
            attributeNames.clear();
        }
        

    }GlobalInfo;
    
    typedef struct {
        unsigned int containerIdx=-1;
        unsigned int songIdx = -1;
        string parsedFile = "";
        int numElements=0;
        Container::SongMeta song;
        map<string,vector<float>> data;
        
    }ContainerBlockInfo;
    
    
    static void init(){globalInfo.clear(); attributeNamesSet = false;};
    virtual bool fillContainerBlock(const string  annotationpath) = 0;
    virtual vector<string> getAttributeNames(const string & path) = 0;
    virtual string getParsedFileCache(const string & file){return file;}
    
    // optional for fast loading (
    static string getGlobalInfoCachePath();
    static bool hasGlobalInfo() ;
    static void saveGlobalInfo();
    static void setGlobalInfo();
    

    int maxAnalysingThread = 6;
    int maxImportingThread = 6;
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

    
    string searchAudiofromAnal(const string & s,const string & audioFolder);


protected:
    
    // return actual number of containerCreated
    virtual int loadFile() = 0;
    
    static  loaders_map_type * loadersMap;
    
    void setSongInfo();
    
    static string cacheName;
    
    static bool attributeNamesSet;
    
};

template<typename T> BaseFileLoader * createT(const string & s,bool isCaching) { return new T(s,isCaching); }




#endif /* defined(__ViZa__BaseFileLoader__) */
