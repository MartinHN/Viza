//
//  JsonLoader.cpp
//  ViZa
//
//  Created by martin hermant on 09/10/14.
//
//

#include "JsonLoader.h"
#define NUM_BEST 30


JsonLoader::JsonLoader(const std::string& name):BaseFileLoader(name){
    
}



bool JsonLoader::loadFile(){
    
    int containerNum = containerBlock.startIdx;
    
    ofxJSONElement json;
    json.open(containerBlock.annotationPath);
    
    
    
    unordered_map<string,vector<float> > onsets;
    crawl(json.get("onsets",NULL),onsets);
    
    
    
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
            
            Container::containers[containerNum] = new Container(containerBlock.annotationPath,containerBlock.audioPath, *(it-1),*it,containerNum);
            //                Container::containers[containerNum]->setAttribute("songIdx",globalCount);
            for(unordered_map<string,vector<float> >::iterator itt=onsets.begin();itt!=onsets.end() ; ++itt){
                if(itt->first!="slice.time" && (subset.size()==0 || ofContains(subset,itt->first ))){
                    Container::containers[containerNum]->setAttribute(itt->first,itt->second[sliceNum]);
                }
                else if (itt->first=="slice.time"){
                    Container::containers[containerNum]->setAttribute("length",*it - *(it-1));
                    Container::containers[containerNum]->setAttribute("startTime",*(it-1));
                    Container::containers[containerNum]->setAttribute("relativeStartTime",onsets["slice.time"].back()!=0?*(it-1)/(onsets["slice.time"].back()):0);
                }
                
            }
            
            for(map<string,string>::iterator itc = classMap.begin() ; itc !=classMap.end() ; ++itc){
                Container::containers[containerNum]->setClass(itc->first, itc->second);
            }
            
            
            containerNum++;
        }
        
        
    
        
        
    }
    //            json.clear();
}


bool JsonLoader::fillContainerBlock(const string &  annotationPath,bool getAttributesNames){
    
    containerBlock.annotationPath = annotationPath;
    
    ofxJSONElement json;
    json.open(annotationPath);
    ofxJSONElement::Value onsets = json["onsets"];
    containerBlock.numSlices = onsets["slice"]["time"].size() - 1;
    string firstValidName= "";
    ofxJSONElement::Value::Members names = onsets.getMemberNames();
    for(int i = 0 ; i < names.size() ,firstValidName==""; i++){
        if(names[i]!="slice"){
            firstValidName = names[i];}
    }
    if(getAttributesNames){
        for(int i = 0 ; i < names.size() ; i++){
            string attrname = names[i];
            if(attrname!="slice"){
                Json::Value::iterator itB = onsets[attrname].begin();
                Json::Value::iterator itE = onsets[attrname].end();
                for (Json::Value::iterator it = itB ; it != itE ; ++it ){
                    
                    string attrtype =it.memberName();
                    containerBlock.attributeNames.push_back(attrname + "." + attrtype);
                }
            }
        }
        
    }
    containerBlock.numAttributes = (onsets.size() - 1)* onsets[firstValidName].size();
    // cout << containerBlock.numAttributes << " : " << containerBlock.numSlices << endl;
    //look for audio path in json file
    string apath = "";
    if(json.get("audiopath",NULL)!=NULL){
        apath = json.get("audiopath",NULL).asString();
        if (apath[0] == '.' && apath[1] == '.'){
            ofFile f(annotationPath);
            ofFile ddd = ofFile(f.getEnclosingDirectory() + apath);
            containerBlock.audioPath =  ddd.path();
        }
    }
    
    
    
    
    
}




void JsonLoader::crawl(Json::Value j,unordered_map<string,vector<float> > & RES){
    
    
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


