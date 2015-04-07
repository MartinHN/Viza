//
//  FileImporter.h
//  ViZa
//
//  Created by martin hermant on 23/01/15.
//
//

#ifndef __ViZa__FileImporter__
#define __ViZa__FileImporter__

#include "ofMain.h"

class FileImporter{
    
public:

    static vector<string> all_extensions;
    vector<int> extension_idx;
    
    virtual int numAttributes(string filePath);
    virtual int numSlice();
    
    virtual bool loadFile(string filePath);
    virtual bool savePosition();
    
    
    
    
    
};

#endif /* defined(__ViZa__FileImporter__) */
