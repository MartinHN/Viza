//
//  jsonLoader.cpp
//  ViZa
//
//  Created by martin hermant on 09/10/14.
//
//

#include "jsonLoader.h"
#define NUM_BEST 30

jsonLoader* jsonLoader::inst = NULL;
int jsonLoader::globalCount = 0;





void jsonLoader::loadSegments(string audiopath,string segpath){
    
    if(audiopath==""){
        audiopath = "/Users/mhermant/Documents/Work/Datasets/beatles/audio/wav";
    }
        if(segpath==""){
        segpath ="/Users/mhermant/Documents/Work/Dev/openFrameworks/apps/ViZa/bin/data/MaschineDrum_All_onset_200_4_f2048_h256/";
//      segpath = "/Users/mhermant/Documents/Work/Datasets/IOWA/theremin.music.uiowa.edu/sound files/MIS/Piano_Other/";
    }
    
    ofDirectory ad =ofDirectory(audiopath);
    ad.allowExt("wav");
    ad.listDir();
    vector<ofFile> audioL = ad.getFiles();

    ad = ofDirectory(segpath);
    if(!ad.exists()){
        ofFileDialogResult f = ofSystemLoadDialog("analysisFiles",true);
        ad = ofDirectory(f.filePath);
        if(!ad.exists()){
            ofExit();
        }
    }
    ad.allowExt("seg");
    ad.allowExt("csv");
    ad.allowExt("lab");
    ad.allowExt("json");
    
    ad.listDir();
    vector<ofFile> segL = ad.getFiles();

    globalCount=0;

    

    

    int numContainers = 0;
    //preallorate huge number of segments for speed purposes (will be resized at the end)
    Container::attributesCache.reserve(Container::containers.size()+segL.size() * NUM_SLICE_CACHE_SIZE * NUM_ATTRIBUTE_CACHE_SIZE);
    Container::containers.reserve(Container::containers.size()+segL.size() * NUM_SLICE_CACHE_SIZE);
    
    
    vector < string> subset(0);
    vector < std::pair<float,string> > tmpsubset(0);
    string metapath = segpath+"meta/best.json";
    ofFile Meta(metapath);
    if(Meta.exists()){
        ofxJSONElement json;
        json.open(metapath);
        Json::Value val = json.get("InfoGain", "");
        for( Json::Value::iterator iit = val.begin();iit != val.end(); ++iit){
            tmpsubset.push_back(std::pair<float,string>((*iit).asFloat(),iit.memberName()));
        }
        std::sort(tmpsubset.begin(), tmpsubset.end());
        vector< std::pair<float,string> >::iterator startiit = tmpsubset.begin();
        if(tmpsubset.size()>NUM_BEST){
            startiit = tmpsubset.end() - NUM_BEST;
        }
        
        for( vector< std::pair<float,string> >::iterator iit = startiit ; iit < tmpsubset.end() ; ++iit ){
            subset.push_back(iit->second);
            cout << iit->second << endl;
        }
        
        
    }
    
    
    for(std::vector<ofFile>::iterator p=segL.begin();p!= segL.end();++p){
        int contwatch = numContainers;
        
        
        
        // Csv file
        wng::ofxCsv csv;
        if(p->getExtension() =="seg"){

   
        }
        
        // JSON FIle
        if(p->getExtension() =="json"){
            ofxJSONElement json;
            json.open(p->path());
            
            
            
            unordered_map<string,vector<float> > onsets;
            crawl(json.get("onsets",NULL),onsets);
            
            //look for audio path in json file
            string apath = "";
            if(json.get("audiopath",NULL)!=NULL){
                apath = json.get("audiopath",NULL).asString();
                if (apath[0] == '.' && apath[1] == '.'){
                    ofFile ddd = ofFile(p->getEnclosingDirectory() + apath);
                    apath =  ddd.path();
                }
            }
            
            //
            if(onsets["slice.time"].size()>1){
                int sliceNum = 0;
                
                map<string,string> classMap;
                // parse Class
                Json::Value classes =json.get("Class","");
                for (Json::Value::iterator it = classes.begin() ; it != classes.end() ; ++it ){
                    string className = it.memberName();
                    string classValue;
                    if((*it).isString()){
                    classValue = (*it).asString();
                    }
                    else cout << "wrongClass defined for " << className <<endl;
                    classMap[className]=classValue;
                    
                }
                
                // add a container per slice
                // ATM only one slice domain is supported, so we need to have one of each descriptor value per slice for all descriptors
                
                // iterate over slices
            for(vector<float>::iterator it  = onsets["slice.time"].begin()+1 ; it!= onsets["slice.time"].end() ; ++it){
                Container::containers.push_back(new Container(p->path(),apath, *(it-1),*it,numContainers));
//                Container::containers.back()->setAttribute("songIdx",globalCount);
                for(unordered_map<string,vector<float> >::iterator itt=onsets.begin();itt!=onsets.end() ; ++itt){
                    if(itt->first!="slice.time" && (subset.size()==0 || ofContains(subset,itt->first ))){
                        Container::containers.back()->setAttribute(itt->first,itt->second[sliceNum]);
                    }
                    else if (itt->first=="slice.time"){
                        Container::containers.back()->setAttribute("length",*it - *(it-1));
                        Container::containers.back()->setAttribute("startTime",*(it-1));
                        Container::containers.back()->setAttribute("relativeStartTime",onsets["slice.time"].back()!=0?*(it-1)/(onsets["slice.time"].back()):0);
                    }
                    
                }
                
                for(map<string,string>::iterator itc = classMap.begin() ; itc !=classMap.end() ; ++itc){
                    Container::containers.back()->setClass(itc->first, itc->second);
                }
                
                
                numContainers++;
                sliceNum++;
            }
                
                
                
                

            }
//            json.clear();
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


void jsonLoader::savePosition(){
    for(map<string,vector<Container*> >::iterator it = Container::songs.begin() ;it != Container::songs.end() ; ++it){
        string outPath = Container::annotationPaths[it->first];
        ofxJSONElement json;
        json.open(outPath);
        for(int i = 0; i < it->second.size() ; i++){
            ofVec3f p = it->second[i]->getPos();
            json["Viza"].clear();
            for(int j = 0 ; j < 3 ; j++){
                json["Viza"].append(p[j]);
            }
        }
        ofFile f(outPath);
        ofDirectory d = f.getEnclosingDirectory();
        string finalPath = d.getAbsolutePath() +"/Viza/"+ f.getFileName();
        json.save(finalPath);
    }
    
    
    
    
    
}


void jsonLoader::crawl(Json::Value j,unordered_map<string,vector<float> > & RES){
    
    
    Json::Value::iterator itB = j.begin();
    Json::Value::iterator itE = j.end();
    for (Json::Value::iterator it = itB ; it != itE ; ++it ){

        string attrname =it.memberName();
        Json::Value::iterator ittB = (*it).begin();
        Json::Value::iterator ittE = (*it).end();
                for (Json::Value::iterator itt = ittB ; itt != ittE ; ++itt ){
                            string attrtype =itt.memberName();
                    Json::Value::iterator itttB = (*itt).begin();
                    Json::Value::iterator itttE = (*itt).end();
                    RES[attrname+"."+attrtype] = std::vector<float>();
                    RES[attrname+"."+attrtype].reserve((*itt).size());
                    for (Json::Value::iterator ittt = itttB ; ittt != itttE ; ++ittt ){
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
            
}




jsonLoader * jsonLoader::instance(){
    if(inst==NULL){ inst =new  jsonLoader;
       }
        return inst;
    
}


int jsonLoader::getValidAttributes(Json::Value onsets){
    
//    onsets["slice.time"]
    
}
