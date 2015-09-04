//
//  AudioLoader.h
//  ViZa
//
//  Created by martin hermant on 09/10/14.
//
//

#ifndef __ViZa__AudioLoader__
#define __ViZa__AudioLoader__

#include <iostream>
//#include "stdio.h"

#include "BaseFileLoader.h"

#include "SimpleEssentiaExtractor.h"


#include "ofxJSONElement.h"



class AudioLoader : public BaseFileLoader{
public:
    
    AudioLoader(const std::string& name);
    
    
    
    bool getCachedInfo(const string & annotationdir)override;
    bool fillContainerBlock(const string & annotationPath) override;
    bool hasCachedInfo(const string & annotationpath) override;
    bool cacheInfo(const string & annotationpath) override;
    
protected:
    int loadFile() override;
    
    
    
private:
    
    SimpleEssentiaExtractor * extr;
    SimpleEssentiaExtractor::ExtractorMap mapIt;
    SimpleEssentiaExtractor::ExtractorInfos infos;
    //    bool evaluateContainersSize(const string & annotationdir);
    string getCachePath(const string & dir);
    bool readMetaFile(const string & annotationdir);
    void  crawl(Json::Value j,unordered_map<string,vector<float> > & RES);
    static string cacheName;
};

#endif /* defined(__ViZa__AudioLoader__) */
