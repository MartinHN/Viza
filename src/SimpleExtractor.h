//
//  SimpleExtractor.h
//  ViZa
//
//  Created by martin hermant on 03/09/15.
//
//

#ifndef __ViZa__SimpleExtractor__
#define __ViZa__SimpleExtractor__

#include <iostream>
#include "BaseExtractor.h"
#include "SimpleEssentiaExtractor.h"
#include "ofMain.h"

class SimpleExtractor : public BaseExtractor{
public:
    
    
    
    void init() override;
    
    static string tmpDir ;
    
    SimpleEssentiaExtractor::ExtractorInfos infos;
    SimpleEssentiaExtractor::ExtractorMap mapIt;
    void setInput(fileInputInfo & f) override;
    virtual void compute()override ;
    virtual void readToSystem()override{};
    
    SimpleEssentiaExtractor  * extr ;
    

    DECLARE_EXTRACTOR(SimpleExtractor);
};

string SimpleExtractor::tmpDir = ofToDataPath("");



#endif /* defined(__ViZa__SimpleExtractor__) */
