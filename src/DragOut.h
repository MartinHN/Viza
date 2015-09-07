//
//  DragOut.h
//  ViZa
//
//  Created by Martin Hermant on 06/09/2015.
//
//

#ifndef ViZa_DragOut_h
#define ViZa_DragOut_h
#include <vector>
#include <string>

#ifdef __OBJC__
#import <Foundation/Foundation.h>
#endif


class DragOut{
    
public:
    DragOut(){};
    ~DragOut(){};
    
    
    static DragOut * i(){
        static DragOut * instance = new DragOut();
        return instance;
    }
    
    bool performExternalDragDropOfFiles (const std::vector<std::string>& files, void * window,float x, float y);

};

#endif
