//
//  BaseFileLoader.cpp
//  ViZa
//
//  Created by martin hermant on 07/04/15.
//
//

#include "BaseFileLoader.h"


#include "JsonLoader.h"

#include "AudioExtractor.h"
#include "AudioJSONExtractor.h"

#ifdef PROTOBUF_SUPPORT
#include "ProtoLoader.h"
#endif

#define AUDIOALGO AudioJSONExtractor

vector<string> BaseFileLoader::attrSubset(0);
BaseFileLoader::loaders_map_type * BaseFileLoader::loadersMap;
bool BaseFileLoader::init ;
string BaseFileLoader::audioFolderPath = "";
string BaseFileLoader::annotationFolderPath = "";
BaseFileLoader::GlobalInfo BaseFileLoader::globalInfo;



BaseFileLoader::BaseFileLoader(const std::string& _name):name(_name){
    ofLogVerbose("FileLoader") << "creating : " << name;
    
}

BaseFileLoader::~BaseFileLoader(){
    ofLogVerbose("FileLoader") << "deleting : " << name;
}

void BaseFileLoader::runTask(){

    if(containerBlock->isCaching){
        fillContainerBlock(containerBlock->parsedFile);
    }
    else{
        if(loadFile() == 0){
            ofLogWarning("BaseFileLoader") << "not found anything for " << containerBlock->parsedFile ;
        }
        else{
            
            setSongInfo();
        }
    }
    
}

void BaseFileLoader::setSongInfo(){
    
    ofScopedLock lock (Container::staticContainerMutex);
    
    containerBlock->song.audioPath = searchAudiofromAnal(containerBlock->parsedFile, audioFolderPath);
    if( containerBlock->song.audioPath==""){
        ofLogError("FileLoader") <<"nothing found for song : "<<containerBlock->parsedFile << " in folder :" << audioFolderPath;
    }
    int locSongIdx = containerBlock->songIdx;
    int locContIdx = containerBlock->containerIdx;
    Container::songMeta[locSongIdx] = containerBlock->song;
    string name = containerBlock->song.name;
    Container::songMeta[locSongIdx].idx = locSongIdx;
    Container::songMeta[locSongIdx].annotationPath=containerBlock->song.annotationPath;
    
    for (int i =  locContIdx ; i < locContIdx+containerBlock->song.numSlices ; i++){
        Container::songsContainers[locSongIdx].push_back(Container::containers[i]->globalIdx);
        Container::containers[i]->songIdx = locSongIdx;
    }
    
    if(!globalInfo.hasVizaMeta){
        for (int i =  locContIdx ; i < locContIdx+containerBlock->song.numSlices ; i++){
            Container::containers[i]->setClass("songName",containerBlock->song.name);
        }
        
    }
    locContIdx+=containerBlock->song.numSlices;
    locSongIdx++;
    
}

vector<string> BaseFileLoader::getAllowedExtensions(){
    vector<string > res ;
    
    for( loaders_map_type::iterator it = getMap()->begin() ; it != getMap()->end() ; ++it){
        res. push_back(it->first);
    }
    return res;
}


void BaseFileLoader::linkLoaders(){
    getMap()->insert(std::make_pair(".json", &createT<JsonLoader>));
#ifdef PROTOBUF_SUPPORT
    getMap()->insert(std::make_pair(".vizad", &createT<ProtoLoader>));
#endif
    getMap()->insert(std::make_pair(".wav", &createT<AUDIOALGO>));
    getMap()->insert(std::make_pair(".mp3", &createT<AUDIOALGO>));
}

BaseFileLoader * BaseFileLoader::getLoader(const string &extension,const string & name){
    
    loaders_map_type::iterator it = getMap()->find(extension);
    if(it == getMap()->end())
        return 0;
    return it->second(extension + " : " + name);
    
}

BaseFileLoader::loaders_map_type * BaseFileLoader::getMap() {
    // never delete'ed. (exist until program termination)
    // because we can't guarantee correct destruction order
    if(!loadersMap) { loadersMap = new loaders_map_type; }
    return loadersMap;
}


string BaseFileLoader::searchAudiofromAnal(const string & s,const string & audioFolder){
    string res = "";
    string name = ofFile(s).getBaseName();
    if(audioFolder!=""){
        string testPath = ofFilePath::addTrailingSlash(audioFolder) + name;
        // test direct correspondance with wav and mp3
        if(ofFile::doesFileExist(testPath+".wav")){
            return testPath+".wav";
        }
        
        else if(ofFile::doesFileExist(testPath+".mp3")){
            return testPath+".mp3";
        }
        
        // else look in subfolders
        else{
            
            
            
            vector<std::filesystem::path > fL =  FileUtils::getFolderPaths(audioFolder);
            for( auto f:fL){
                res = searchAudiofromAnal(s,f.string());
                if(res!=""){
                    return res;
                }
            }
        }
    }
    
    return res;
}





