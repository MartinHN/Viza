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
#include "Container.h"
#include "Poco/Task.h"
#include "Poco/Notification.h"
#include "Poco/Random.h"




class BaseFileLoader : public Poco::Task{
    
public:
    
    typedef map<string,BaseFileLoader*(*)(const string & s)> loaders_map_type;
    
    static BaseFileLoader* getLoader(string const & extension,string const & name);
    static loaders_map_type * getMap();
    
    BaseFileLoader(const std::string& name);
    virtual ~BaseFileLoader();
    vector<string> extensions;
    
    typedef struct {
        int startIdx;
        
        int numAttributes;
        int numSlices;
        string audioPath;
        string annotationPath;
        
        vector<string> attributeNames;
        
    }ContainerBlock;
    
    
    virtual bool fillContainerBlock(const string & annotationpath,bool getAttributeNames) = 0;
    ContainerBlock containerBlock;


    
    static void linkLoaders();
    
    void runTask();
    
    
    static vector<string> subset;
    
    
protected:
    virtual bool loadFile() = 0;
    
    static  loaders_map_type * loadersMap;
    
    
    
};

template<typename T> BaseFileLoader * createT(const string & s) { return new T(s); }




#endif /* defined(__ViZa__BaseFileLoader__) */
