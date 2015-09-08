//
//  AudioJSONExtractor.cpp
//  ViZa
//
//  Created by martin hermant on 09/10/14.
//
//

#include "AudioJSONExtractor.h"
#define NUM_BEST 30



bool AudioJSONExtractor::fillContainerBlock(const string & annotationPath){
    
    
    extr = new SimpleEssentiaExtractor(infos);
    extr->statsToCompute = statsToCompute;
    extr->initFile();
    extr->buildMe();
    extr->setInput(containerBlock->parsedFile,"",map<string,string>());
    extr->threadedFunction();

    
    containerBlock->song.audioPath = containerBlock->parsedFile;
    vector<Real> onsets = extr->aggregatedPool.value<vector<Real>>("onsets");
    containerBlock->numElements = onsets.size();
    containerBlock->song.length = extr->outPool.value<Real>("metadata.duration");
    containerBlock->song.numSlices = onsets.size();
    containerBlock->song.name = ofFile(annotationPath).getBaseName();
    
        // Save to json to avoid to much heap memory alloc before loadFiles

    ofFile pFile(containerBlock->parsedFile);
    string destinationFile = ofFilePath::join(pFile.getEnclosingDirectory(),"Viza");
    destinationFile = ofFilePath::join(destinationFile, pFile.getBaseName() + ".json");//getAbsolutePath();
    ofFile(destinationFile).create();
    ofLogWarning("FileLoader") << "saving JSON at : " << destinationFile;
    extr->saveIt(destinationFile);
    containerBlock->parsedFile = destinationFile;
    
    
    
    extr->aggregatedPool.clear();
    
       delete extr;
}

int AudioJSONExtractor::loadFile(){
    
    ofxJSONElement json;
    json.open(containerBlock->parsedFile);
    
    vector<std::pair<float,float>> onsets;
    for(int i = 0; i <  json["slice"]["time"].size() ; i++){
        onsets.push_back(std::pair<float,float>(json["slice"]["time"][i][0].asFloat(),json["slice"]["time"][i][1].asFloat()));
    };
 


    int containerNum = containerBlock->containerIdx;
    
    
    float begin = 0;
    float end = containerBlock->song.length;
    for(int i = 0 ; i < containerBlock->numElements ; i++){
        
        Container::containers[containerNum] = new Container(begin,end,containerNum,0);
        begin = onsets[i].first;
        end = onsets[i].second;
        
        
        
        for(auto & v:mapIt.outputs){
            string name = "values."+v+".mean";
            ofxJSONElement value = json["values"][v]["mean"].get(i, 0);
            if(!value.isNumeric()){
                ofLogError("AudioLoader")<<" No feature found " << name;
                           for (auto d:value){
                               ofLogError("AudioLoader") << "\t" << d.type();
                           }
            }
            else{
            
            Container::containers[containerNum]->setAttribute(v,value.asFloat());
            }
        }
        
        containerNum++;
    }
    
    
    //    for(map<string,vector<string> >::iterator itc = classMap.begin() ; itc !=classMap.end() ; ++itc){
    //        Container::containers[containerNum]->setClass(itc->first, itc->second[sliceNum]);
    //    }
    //    delete res;
    return 1;
    
    
    
    
    
};


