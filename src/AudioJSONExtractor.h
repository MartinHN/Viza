//
//  AudioJSONExtractor.h
//  ViZa
//
//  Created by martin hermant on 09/10/14.
//
//

#ifndef __ViZa__AudioJSONExtractor__
#define __ViZa__AudioJSONExtractor__

#include <iostream>
//#include "stdio.h"



#include "AudioExtractor.h"





class AudioJSONExtractor : public AudioExtractor{
public:
    
    
    AudioJSONExtractor(const std::string& name):AudioExtractor(name){
    };

    bool fillContainerBlock(const string  annotationPath) ;

    
protected:
    int loadFile() ;
    
    
    
private:
    

};

#endif /* defined(__ViZa__AudioJSONExtractor__) */
