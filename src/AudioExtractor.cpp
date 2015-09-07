//
//  AudioExtractor.cpp
//  ViZa
//
//  Created by martin hermant on 09/10/14.
//
//

#include "AudioExtractor.h"
#define NUM_BEST 30


string AudioExtractor::cacheName = "Viza/_vizameta.json";
vector<string> AudioExtractor::statsToCompute;
bool AudioExtractor::bEssentiaInited = false;

AudioExtractor::AudioExtractor(const std::string& name):BaseFileLoader(name){
    {
        
    ofScopedLock(staticMutex);
    if(!bEssentiaInited){

        essentia::init();
        bEssentiaInited =true;
        
    }
    }
    
    SupportedNumThreads = 1;
    extensions = vector<string>();
    extensions.push_back(".wav");
    extensions.push_back(".mp3");
    //    essentia::setDebugLevel(essentia::DebuggingModule::EExecution);
    
    
    mapIt.name = "LowLevelSpectralExtractor";
    mapIt.inputName = "signal";
//    mapIt.parameters.add("frameSize", 1024);
//    mapIt.parameters.add("hopSize",512);
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
    
    statsToCompute = vector<string>(1,"mean");
    
}


bool AudioExtractor::fillContainerBlock(const string & annotationPath){
    
    
    extr = new SimpleEssentiaExtractor(infos);
    extr->statsToCompute = statsToCompute;
    extr->initFile();
    extr->buildMe();
    extr->setInput(containerBlock->parsedFile,"",map<string,string>());
    extr->threadedFunction();
    
    containerBlock->numElements = 1;
    containerBlock->song.length = extr->outPool.value<Real>("metadata.duration");
    containerBlock->song.audioPath = annotationPath;
    containerBlock->song.numSlices = 1;
    containerBlock->song.name = ofFile(annotationPath).getBaseName();
    
    

    containerBlock->data = extr->aggregatedPool.getSingleRealPool();
    delete extr;
}

int AudioExtractor::loadFile(){
    
//    containerBlock->data;
//    if(res == nullptr){
//        ofLogError("AudioLoader") << "corrupted data Pointer ";
//        
//        return;
//    }
//    
    int containerNum = containerBlock->containerIdx;
    
    float begin = 0;
    float end = containerBlock->song.length;
    
    Container::containers[containerNum] = new Container(begin,end,containerNum,0);
    
    
    for(auto & v:mapIt.outputs){
        string name = "values."+v+".mean";
        if(containerBlock->data.count(name)==0){
            ofLogError("AudioLoader")<<" No feature found " << name;
//            for (auto d:containerBlock->data){
//                ofLogError("AudioLoader") << "\t" << d.first;
//            }
        }
        Container::containers[containerNum]->setAttribute(v,containerBlock->data.at(name));
    }
    
    
    // Add Meta Info
    Container::containers[containerNum]->setAttribute("length",end-begin);
    Container::containers[containerNum]->setAttribute("startTime",begin);
    Container::containers[containerNum]->setAttribute("relativeStartTime",containerBlock->song.length!=0?begin/(containerBlock->song.length):0);
    
    
    //    for(map<string,vector<string> >::iterator itc = classMap.begin() ; itc !=classMap.end() ; ++itc){
    //        Container::containers[containerNum]->setClass(itc->first, itc->second[sliceNum]);
    //    }
//    delete res;
    return 1;
    
    
    
    
    
};






bool AudioExtractor::hasCachedInfo(){
    return false;// getCachePath(BaseFileLoader::annotationFolderPath)!="";
}

string AudioExtractor::getCachePath(const string & annotationpath){
    ofFile cache;
    cache.open(annotationpath + "/"+cacheName);
    return cache.exists()?cache.path():"";
    
}

bool AudioExtractor::getCachedInfo(const string &annotationdir){
    
    
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


vector<string> AudioExtractor::getAttributeNames(const string & path){
    return mapIt.outputs;
    
    
}
int AudioExtractor::cacheInfo(){
    
    
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
            ofLogError("AudioExtractor", "cant import :" +f.path());
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



