//
//  jsonLoader.cpp
//  ViZa
//
//  Created by martin hermant on 09/10/14.
//
//

#include "jsonLoader.h"


jsonLoader* jsonLoader::inst = NULL;
int jsonLoader::globalCount = 0;





void jsonLoader::loadSegments(string audiopath,string segpath){
    
    if(audiopath==""){
        audiopath = "/Users/mhermant/Documents/Work/Datasets/beatles/audio/wav";
    }
        if(segpath==""){
        segpath ="/Users/mhermant/Documents/Work/Dev/openFrameworks/apps/ViZa/bin/data/tests4/";
//      segpath = "/Users/mhermant/Documents/Work/Datasets/IOWA/theremin.music.uiowa.edu/sound files/MIS/Piano_Other/";
    }
    
    ofDirectory ad =ofDirectory(audiopath);
    ad.allowExt("wav");
    ad.listDir();
    vector<ofFile> audioL = ad.getFiles();

    ad = ofDirectory(segpath);
    ad.allowExt("seg");
    ad.allowExt("csv");
    ad.allowExt("lab");
    ad.allowExt("json");
    
    ad.listDir();
    vector<ofFile> segL = ad.getFiles();

    globalCount=0;

    
    //Create map of audio and datafiles
    std::map<ofFile,ofFile> mapL;
    
    for(int i = 0 ; i < segL.size();i++){
        bool found = false;
            for(int j = 0 ; j < audioL.size();j++){
                if(audioL[j].getBaseName()==segL[i].getBaseName()){
                    mapL[audioL[j]] = segL[i];
                    found = true;
                    break;
                }
            }
        // still try if file name is in jsonFile
        if(!found)mapL[segL[i]] = segL[i];
    }
    
    int numContainers = 0;
    //preallorate huge number of segments for speed purposes (will be resized at the end)
    Container::attributesCache.reserve(Container::containers.size()+mapL.size() * NUM_SLICE_CACHE_SIZE * NUM_ATTRIBUTE_CACHE_SIZE);
    Container::containers.reserve(Container::containers.size()+mapL.size() * NUM_SLICE_CACHE_SIZE);
    
    
    for(std::map<ofFile, ofFile>::iterator p=mapL.begin();p!= mapL.end();++p){
        int contwatch = numContainers;
        
        
        
        // Csv file
        wng::ofxCsv csv;
        if(p->second.getExtension() =="seg"){
//        csv.loadFile(p->second.path(), "\t");
//        
//        for(int i = 0 ; i < csv.numRows ; i++){
//            Container::containers.push_back(Container(p->first.path(), csv.getFloat(i, 0), csv.getFloat(i, 1),numContainers));
//            numContainers++;
//            
//            
//        }
   
        }
        
        // JSON FIle
        if(p->second.getExtension() =="json"){
            ofxJSONElement json;
            json.open(p->second.path());
            
            
            
            map<string,vector<float> > onsets = crawl(json.get("onsets",NULL));
            
            //look for audio path in json file
            string apath = "";
            if(json.get("audiopath",NULL)!=NULL){
                apath = json.get("audiopath",NULL).asString();
            }
            
            //
            if(onsets["slice.time"].size()>1){
                int sliceNum = 0;
                
                
                
                // add a container per slice
                // ATM only one slice domain is supported, so we need to have one of each descriptor value per slice for all descriptors
                
                // itereate over slices
            for(vector<float>::iterator it  = onsets["slice.time"].begin()+1 ; it!= onsets["slice.time"].end() ; ++it){
                Container::containers.push_back(new Container(apath!=""?apath:p->first.path(), *(it-1),*it,numContainers));
                Container::containers.back()->setClass("songIdx",ofToString(globalCount));
                for(map<string,vector<float> >::iterator itt=onsets.begin();itt!=onsets.end() ; ++itt){
                    if(itt->first!="slice.time"){
                        Container::containers.back()->setAttribute(itt->first,itt->second[sliceNum]);
                    }
                    else{
                        Container::containers.back()->setAttribute("length",*it - *(it-1));
                        Container::containers.back()->setAttribute("startTime",*(it-1));
                        Container::containers.back()->setAttribute("relativeStartTime",onsets["slice.time"].back()!=0?*(it-1)/(onsets["slice.time"].back()):0);
                    }
                    
                }
                
                
                numContainers++;
                sliceNum++;
            }
            }
            json.clear();
        }
        
//        if found increment container count
        if( contwatch != numContainers){
        globalCount++;
        }
       
        
        
        
    }
    
    Container::containers.resize(numContainers);
    Container::attributesCache.resize(numContainers*Container::attrSize);

    Container::CacheNormalized(numContainers);
    
}


map<string,vector<float> > jsonLoader::crawl(Json::Value j){
    
    map<string,vector<float> >  RES;
    
    for (Json::Value::iterator it = j.begin() ; it != j.end() ; ++it ){

        string attrname =it.memberName();
                for (Json::Value::iterator itt = (*it).begin() ; itt != (*it).end() ; ++itt ){
                            string attrtype =itt.memberName();
                    for (Json::Value::iterator ittt = (*itt).begin() ; ittt != (*itt).end() ; ++ittt ){
                        if((*ittt).isNumeric())
                        RES[attrname+"."+attrtype].push_back((*ittt).asFloat());
                        else if ((*ittt).isArray()){
                            int i = 0;
                            for (Json::Value::iterator itttt = (*ittt).begin() ; itttt != (*ittt).end() ; ++itttt ){
                               RES[attrname+"."+attrtype+"."+ofToString(i)].push_back((*itttt).asFloat());
                                i++;
                            }

                        }
                            
                    }
                    
                }
    
    }
               return RES;
}




jsonLoader * jsonLoader::instance(){
    if(inst==NULL){ inst =new  jsonLoader;
       }
        return inst;
    
}


int jsonLoader::getValidAttributes(Json::Value onsets){
    
//    onsets["slice.time"]
    
}
