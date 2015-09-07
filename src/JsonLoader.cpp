//
//  JsonLoader.cpp
//  ViZa
//
//  Created by martin hermant on 09/10/14.
//
//

#include "JsonLoader.h"
#define NUM_BEST 30


string JsonLoader::cacheName = "Viza/_vizameta.json";

JsonLoader::JsonLoader(const std::string& name):BaseFileLoader(name){
    extensions = vector<string>(1,".json");
}



int JsonLoader::loadFile(){
    
    int containerNum = containerBlock->containerIdx;

    ofxJSONElement json;
    int numCreated = 0;
    //only one file at time
    Container::SongMeta song = containerBlock->song;
    

    string name = containerBlock->song.name;
    json.open(song.annotationPath);
    

    
    unordered_map<string,vector<float> > onsets;
    crawl(json.get("values",0),onsets);
    
    
    
    
    
    
    map<string,vector<string> > classMap;
    // parse Class
    Json::Value classes =json.get("Class","");
    for (Json::Value::iterator it = classes.begin() ; it != classes.end() ; ++it ){
        string className = it.memberName();
        string classValue;
        if((*it).isString()){
            classValue = (*it).asString();
        }
        else if((*it).isArray()){
            ofLogError("JsonLoader","class Array not Supported Yet");
        }
        else ofLogError("JsonLoader", "wrongClass defined for " +className);
        if(!classMap.count(className))classMap[className] = vector<string>();
        classMap[className].push_back(classValue);
        
    }
    
    // either we provide slices timestamps, either total duration (one slice files)
    
    vector < pair<float,float> > slices;
    if(json.isMember("slice") && json["slice"]["time"].size()>0){
        for(Json::Value::iterator it  = json["slice"]["time"].begin() ; it!= json["slice"]["time"].end() ; ++it){
            slices.push_back(pair<float,float>((*it)[0].asFloat(),(*it)[1].asFloat()));
        }
    }
    else if(json.isMember("metadata") && json["metadata"].isMember("duration") ){
        slices.push_back(pair<float,float>(0,json["metadata"]["duration"].asFloat()));
    }
    
    
    if(slices.size()>0){
        // add a container per slice
        // ATM only one slice domain is supported, so we need to have one of each descriptor value per slice for all descriptors
        
        // iterate over slices
        int sliceNum = 0;
        for(vector<pair<float,float> >::iterator it  = slices.begin() ; it!= slices.end() ; ++it){
            float begin = it->first;
            float end = it->second;
            
            Container::containers[containerNum] = new Container(begin,end,containerNum,sliceNum);
            numCreated++;
            
            for(unordered_map<string,vector<float> >::iterator itt=onsets.begin();itt!=onsets.end() ; ++itt){
                if(itt->first!="slice.time" && (attrSubset.size()==0 || ofContains(attrSubset,itt->first ))){
                    Container::containers[containerNum]->setAttribute(itt->first,itt->second[sliceNum]);
                }
                else{
                    ofLogVerbose("JSON Importer")<<"ignoring : " << itt->first;
                }
                
                
            }
            // Add Meta Info
            Container::containers[containerNum]->setAttribute("length",end-begin);
            Container::containers[containerNum]->setAttribute("startTime",begin);
            Container::containers[containerNum]->setAttribute("relativeStartTime",containerBlock->song.length!=0?begin/(containerBlock->song.length):0);
            
            
            for(map<string,vector<string> >::iterator itc = classMap.begin() ; itc !=classMap.end() ; ++itc){
                Container::containers[containerNum]->setClass(itc->first, itc->second[sliceNum]);
            }
            
            sliceNum++;
            
            containerNum++;
            
        }
        
        
        
        
        
    }
    else{
        ofLogError("FileImporter", "Cant find slices or metadata.duration in json file : "+ song.annotationPath);
    }
    
    return numCreated;

}


bool JsonLoader::fillContainerBlock(const string &  annotationPath){
    
    ofxJSONElement json;
    json.open(annotationPath);
    ofxJSONElement::Value onsets = json["values"];
    
    
    
    
    if(json.isMember("slice")){
        containerBlock->numElements = json["slice"]["time"].size();
        containerBlock->song.length = json["slice"]["time"][containerBlock->numElements][1].asFloat();
    }
    else if(json.isMember("metadata") && json["metadata"].isMember("duration") ){
        containerBlock->numElements = 1;
        containerBlock->song.length = json["metadata"]["duration"].asFloat();
    }
    else{
        ofLogError("FileImporter","no duration found for : " + annotationPath );
    }
    
    containerBlock->song.numSlices= containerBlock->numElements;
    containerBlock->song.annotationPath = annotationPath;
    containerBlock->song.name = ofFile(annotationPath).getBaseName();


}



bool JsonLoader::hasCachedInfo(){
    return getCachePath(BaseFileLoader::annotationFolderPath)!="";
}

string JsonLoader::getCachePath(const string & annotationpath){
    ofFile cache;
    cache.open(annotationpath + "/"+cacheName);
    return cache.exists()?cache.path():"";
    
}

bool JsonLoader::getCachedInfo(const string &annotationdir){

    
    ofxJSONElement json;
    json.open(getCachePath(annotationdir));

    globalInfo.attributeNames.clear();
    globalInfo.attributeNames.reserve(json["attributeNames"].size());\
    for (Json::Value::iterator it = json["attributeNames"].begin() ; it != json["attributeNames"].end() ; ++it ){ \
        globalInfo.attributeNames.push_back((*it).asString());
    }
    
    // contain Viza-added Attribute names : length, start idx, relativeStartidx
    globalInfo.hasVizaMeta = json.get("hasVizaMeta",false).asBool();
    globalInfo.totalContainers = json.get("totalContainers",0).asInt64();
    
    return globalInfo.totalContainers!=0;
    
}


vector<string> JsonLoader::getAttributeNames(const string & path){
    vector<string> res;
    ofxJSONElement json;
    json.open(path);
    ofxJSONElement::Value onsets = json["values"];
    ofxJSONElement::Value::Members names = onsets.getMemberNames();
    for(int i = 0 ; i < names.size() ; i++){
        string attrname = names[i];
        if(onsets[attrname].isNumeric()||onsets[attrname].isArray()){
            res.push_back(attrname );
        }
        else{
            Json::Value::iterator itB = onsets[attrname].begin();
            Json::Value::iterator itE = onsets[attrname].end();
            for (Json::Value::iterator it = itB ; it != itE ; ++it ){
                
                string attrtype =it.memberName();
                res.push_back(attrname + "." + attrtype);
            }
        }
        
    }
    return res;
}
int JsonLoader::cacheInfo(){
    
    
    GlobalInfo fakeInfo ;
    fakeInfo.hasVizaMeta = false;
    ofDirectory directory(BaseFileLoader::annotationFolderPath);
    directory.allowExt("json");
    vector <ofFile> jsons =  directory.getFiles();

    if(jsons.size()==0){
        ofLogError("FileImporter","no jsonFile found ");
    }
    

    

    
    
    // fill attribute Names
    
    if(attrSubset.size()){
        fakeInfo.attributeNames = attrSubset;
    }
    else{
    fakeInfo.attributeNames = getAttributeNames(jsons[0].path());
    }
    unsigned int totalContainers = 0;
    ofxJSONElement jsont;
    for (auto & f:jsons){
        if(!jsont.open(f.path()) ){
            ofLogError("JsonLoader", "cant import :" +f.path());
        }
        else{
        totalContainers+= MAX(1,jsont["slice"].size());
        
        }
    }
    
    
    fakeInfo.totalContainers = totalContainers;
    


    // Save It
    ofxJSONElement jsonOut;
    
    jsonOut["hasVizaMeta"] = fakeInfo.hasVizaMeta;
    jsonOut["totalContainers"] = fakeInfo.totalContainers;
    int i = 0;
    jsonOut["attributeNames"].resize(fakeInfo.attributeNames.size());
    for(auto & a:fakeInfo.attributeNames){
        jsonOut["attributeNames"][i] = a;
        i++;
    }


    jsonOut.save(BaseFileLoader::annotationFolderPath + "/"+cacheName);
    
}




void JsonLoader::crawl(Json::Value j,unordered_map<string,vector<float> > & RES){
    
    
    Json::Value::iterator itB = j.begin();
    Json::Value::iterator itE = j.end();
    for (Json::Value::iterator it = itB ; it != itE ; ++it ){
        string attrname =it.memberName();
        // 1 depth json list
        if((*it).isArray()){
            RES[attrname] = std::vector<float>();
            RES[attrname].reserve((*it).size());
            Json::Value::iterator itttB = (*it).begin();
            Json::Value::iterator itttE = (*it).end();
            for (Json::Value::iterator ittt = itttB ; ittt != itttE ; ++ittt ){
                if((*ittt).isNumeric())
                    RES[attrname].push_back((*ittt).asFloat());
                else
                    ofLogError("json value not numerical");
                
            }
            

        }
        // 1 depth json value
        else if ((*it).isNumeric()){
            RES[attrname] = std::vector<float>(1,(*it).asFloat());
        }
        
        else{
        Json::Value::iterator ittB = (*it).begin();
        Json::Value::iterator ittE = (*it).end();
        for (Json::Value::iterator itt = ittB ; itt != ittE ; ++itt ){
            string attrtype =itt.memberName();
            
            if((*itt).isArray()){
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
            else if((*itt).isNumeric()){
                 RES[attrname+"."+attrtype].push_back((*itt).asFloat());
                
            }
            
        }
        }
        
    }
    
}


