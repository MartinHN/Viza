//
//  AudioLoader.cpp
//  ViZa
//
//  Created by martin hermant on 09/10/14.
//
//

#include "AudioLoader.h"
#define NUM_BEST 30


string AudioLoader::cacheName = "Viza/_vizameta.json";

AudioLoader::AudioLoader(const std::string& name):BaseFileLoader(name){
    extensions = vector<string>();
    extensions.push_back(".wav");
    extensions.push_back(".mp3");
    //    essentia::setDebugLevel(essentia::DebuggingModule::EExecution);
    
    
    mapIt.name = "LowLevelSpectralExtractor";
    mapIt.inputName = "signal";
    //    mapIt.outputs.push_back("barkbands");
    mapIt.outputs.push_back("barkbands_kurtosis");
    mapIt.outputs.push_back("barkbands_skewness");
    mapIt.outputs.push_back("barkbands_spread");
    mapIt.outputs.push_back("hfc");
    //    mapIt.outputs.push_back("mfcc");
    mapIt.outputs.push_back("pitch");
    mapIt.outputs.push_back("pitch_instantaneous_confidence");
    mapIt.outputs.push_back("pitch_salience");
    //    mapIt.outputs.push_back("silence_rate_20dB");
    //    mapIt.outputs.push_back( "silence_rate_30dB");
    //    mapIt.outputs.push_back( "silence_rate_60dB");
    mapIt.outputs.push_back( "spectral_complexity");
    mapIt.outputs.push_back("spectral_crest");
    mapIt.outputs.push_back("spectral_decrease");
    mapIt.outputs.push_back("spectral_energy");
    mapIt.outputs.push_back("spectral_energyband_low");
    mapIt.outputs.push_back("spectral_energyband_middle_low");
    mapIt.outputs.push_back("spectral_energyband_middle_high");
    mapIt.outputs.push_back("spectral_energyband_high");
    mapIt.outputs.push_back("spectral_flatness_db");
    mapIt.outputs.push_back("spectral_flux");
    mapIt.outputs.push_back("spectral_rms");
    mapIt.outputs.push_back("spectral_rolloff");
    mapIt.outputs.push_back("spectral_strongpeak");
    mapIt.outputs.push_back("zerocrossingrate");
    mapIt.outputs.push_back("inharmonicity");
    //    mapIt.outputs.push_back("tristimulus");
    //mapIt.outputs.push_back("oddtoevenharmonicenergyratio");
    
    infos.push_back(mapIt);
    
    
    extr = new SimpleEssentiaExtractor(infos);
    
    extr->initFile();
    extr->buildMe();
}



int AudioLoader::loadFile(){
    extr->setInput(containerBlock.parsedFile,"",map<string,string>());
    extr->threadedFunction();
    
    }


bool AudioLoader::fillContainerBlock(const string &  annotationPath){
    
    ofxJSONElement json;
    json.open(annotationPath);
    ofxJSONElement::Value onsets = json["values"];
    
    
    
    if(json.isMember("slice")){
        containerBlock.numElements = json["slice"]["time"].size();
        containerBlock.song.length = json["slice"]["time"][containerBlock.numElements][1].asFloat();
    }
    else if(json.isMember("metadata") && json["metadata"].isMember("duration") ){
        containerBlock.numElements = 1;
        containerBlock.song.length = json["metadata"]["duration"].asFloat();
    }
    else{
        ofLogError("FileImporter","no duration found for : " + annotationPath );
    }
    
    containerBlock.song.numSlices= containerBlock.numElements;
    containerBlock.song.annotationPath = annotationPath;
    containerBlock.song.name = ofFile(annotationPath).getBaseName();
    
    
}



bool AudioLoader::hasCachedInfo(const string &annotationpath){
    return getCachePath(annotationpath)!="";
}

string AudioLoader::getCachePath(const string & annotationpath){
    ofFile cache;
    cache.open(annotationpath + "/"+cacheName);
    return cache.exists()?cache.path():"";
    
}

bool AudioLoader::getCachedInfo(const string &annotationdir){
    
    
    ofxJSONElement json;
    json.open(getCachePath(annotationdir));
    
    
    globalInfo.attributeNames.reserve(json["attributeNames"].size());\
    for (Json::Value::iterator it = json["attributeNames"].begin() ; it != json["attributeNames"].end() ; ++it ){ \
        globalInfo.attributeNames.push_back((*it).asString());
    }
    
    // contain Viza-added Attribute names : length, start idx, relativeStartidx
    globalInfo.hasVizaMeta = json.get("hasVizaMeta",false).asBool();
    globalInfo.totalContainers = json.get("totalContainers",0).asInt64();
    
    return globalInfo.totalContainers!=0;
    
}

bool AudioLoader::cacheInfo(const string & annotationdir){
    
    
    GlobalInfo fakeInfo ;
    fakeInfo.hasVizaMeta = false;
    ofDirectory directory(annotationdir);
    directory.allowExt("json");
    vector <ofFile> jsons =  directory.getFiles();
    
    if(jsons.size()==0){
        ofLogError("FileImporter","no jsonFile found ");
    }
    
    ofxJSONElement json;
    json.open(jsons[0].path());
    ofxJSONElement::Value onsets = json["values"];
    
    
    
    
    // fill attribute Names
    
    if(attrSubset.size()){
        fakeInfo.attributeNames = attrSubset;
    }
    else{
        ofxJSONElement::Value::Members names = onsets.getMemberNames();
        for(int i = 0 ; i < names.size() ; i++){
            string attrname = names[i];
            if(onsets[attrname].isNumeric()||onsets[attrname].isArray()){
                fakeInfo.attributeNames.push_back(attrname );
            }
            else{
                Json::Value::iterator itB = onsets[attrname].begin();
                Json::Value::iterator itE = onsets[attrname].end();
                for (Json::Value::iterator it = itB ; it != itE ; ++it ){
                    
                    string attrtype =it.memberName();
                    fakeInfo.attributeNames.push_back(attrname + "." + attrtype);
                }
            }
            
        }
    }
    
    
    unsigned int totalContainers = 0;
    ofxJSONElement jsont;
    for (auto & f:jsons){
        if(!jsont.open(f.path()) ){
            ofLogError("AudioLoader", "cant import :" +f.path());
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
    
    
    jsonOut.save(annotationdir + "/"+cacheName);
    
}




void AudioLoader::crawl(Json::Value j,unordered_map<string,vector<float> > & RES){
    
    
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


