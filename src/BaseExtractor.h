//
//  BaseExtractor.h
//  ViZa
//
//  Created by martin hermant on 03/09/15.
//
//

#ifndef __ViZa__BaseExtractor__
#define __ViZa__BaseExtractor__

#include "ofMain.h"
#include "Poco/Task.h"
#include "Poco/Notification.h"
#include "Poco/Random.h"


#define DECLARE_EXTRACTOR(NAME) \
static ExtractorFactory<NAME> reg;\
NAME(const string & n):BaseExtractor(n){init();};

#define REGISTER_EXTRACTOR(NAME) \
ExtractorFactory<NAME> NAME::reg(#NAME)

class BaseExtractor :  public Poco::Task{
public:
    
    //////////////////////////////////////////
    // types
    typedef struct fileInputInfo{
        string name;
        string audioPath;
        string outputPath;
        map<string,string> classes;
        int length;
        fileInputInfo(){};
        fileInputInfo(const string  & name,const string & audioP,const string & outP,const map<string,string>& _classes=map<string,string>()):name(name),audioPath(audioP),outputPath(outP),classes(_classes){};
    }fileInputInfo;
    
    
    
    BaseExtractor(const string & name):Poco::Task(name){
    };
    virtual ~BaseExtractor(){};

    // implemnentation specific Extractor method
    virtual void init(){};
    virtual void setInput(fileInputInfo & f){};
    virtual void compute() = 0;
    virtual void readToSystem() = 0;
    
    string inputAudioPath;
    bool handleTimeSlice();
    std::pair<float,float> timeSlice;
    
    void runTask(){};
    
    
};


template<typename T> BaseExtractor * createT(const string & name) { return new T(name); }

struct BaseExtractorFactory {
    typedef std::map<std::string, BaseExtractor*(*)(const string & name)> map_type;
    
    static BaseExtractor * createInstance(std::string const& type,std::string const& name) {
        map_type::iterator it = getMap()->find(type);
        if(it == getMap()->end())
            return 0;
        return it->second(name);
    }
    
protected:
    static map_type * getMap() {
        // never delete'ed. (exist until program termination)
        // because we can't guarantee correct destruction order
        if(!map) { map = new map_type; }
        return map;
    }
    
private:
    static map_type * map;
};


template<typename T>
struct ExtractorFactory : BaseExtractorFactory {
    ExtractorFactory(std::string const& s) {
        getMap()->insert(std::make_pair(s, &createT<T>));
    }
};



#endif /* defined(__ViZa__BaseExtractor__) */
