//
//  AudioExtractor.h
//  ViZa
//
//  Created by martin hermant on 09/10/14.
//
//

#ifndef __ViZa__AudioExtractor__
#define __ViZa__AudioExtractor__

#include <iostream>
//#include "stdio.h"

#include "BaseFileLoader.h"


#include "SimpleEssentiaExtractor.h"

#include "ofxJSONElement.h"




class AudioExtractor : public BaseFileLoader{
public:
    
    AudioExtractor(const std::string& name);
    
    
    bool createExtractor();
    bool getCachedInfo(const string & annotationdir)override;
    bool fillContainerBlock(const string  annotationPath) override;
    bool hasCachedInfo() override;
    int cacheInfo() override;
    void endCaching(){};
    vector<string> getAttributeNames(const string & path)override ;
    
protected:
    int loadFile() override;
    
    SimpleEssentiaExtractor * extr;
    
    

    
    static ofMutex staticMutex;
    
    
    SimpleEssentiaExtractor::ExtractorMap mapIt;
    SimpleEssentiaExtractor::ExtractorInfos infos;
    
    //    bool evaluateContainersSize(const string & annotationdir);
    string getCachePath(const string & dir);
    bool readMetaFile(const string & annotationdir);

    static string cacheName;
    
    static vector<string> statsToCompute;

    static bool bEssentiaInited;
};

#endif /* defined(__ViZa__AudioExtractor__) */
