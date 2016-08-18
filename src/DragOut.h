//
//  DragOut.h
//  ViZa
//
//  Created by Martin Hermant on 06/09/2015.
//
//

#ifndef ViZa_DragOut_h
#define ViZa_DragOut_h


#ifdef __OBJC__
#import <Foundation/Foundation.h>

#endif


#include <vector>
#include <string>
#include "essentia.h"
#include "essentia/algorithm.h"
#include "essentia/algorithmfactory.h"
//#include "ofMain.h"

using namespace std;

class DragOut{
    
public:
    DragOut(){};
    ~DragOut(){};
    
    
    static DragOut * i(){
        static DragOut * instance = new DragOut();
        return instance;
    }
    
    bool performExternalDragDrop (const std::vector<std::string>& files,std::string tmpFolder,const std::vector<float>& starts,const std::vector<float>& ends, void * window,float x, float y);


    bool performExternalDragDropOfFiles(const std::vector<std::string>& files, void * window,float x, float y);


};

#endif
