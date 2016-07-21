//
//  AudioExtractor.cpp
//  ViZa
//
//  Created by martin hermant on 09/10/14.
//
//

#include "AudioExtractor.h"
#define NUM_BEST 30



vector<string> AudioExtractor::statsToCompute;
bool AudioExtractor::bEssentiaInited = false;
AudioExtractor::AlgoType AudioExtractor::type = lowLevel;
ofMutex AudioExtractor::staticMutex;

AudioExtractor::AudioExtractor(const std::string& name,bool isCaching):
BaseFileLoader(name,isCaching),
extr(nullptr){
    {
        
        ofScopedLock sl(staticMutex);
        if(!bEssentiaInited){
            bEssentiaInited =true;
            
        }
    }
    essentia::infoLevelActive = false;
    essentia::warningLevelActive = false;
    essentia::errorLevelActive = true;
    maxAnalysingThread = 4 ;
    maxImportingThread = 4 ;
    extensions = vector<string>();
    extensions.push_back(".wav");
    extensions.push_back(".mp3");
//    essentia::setDebugLevel(essentia::DebuggingModule::EMemory );

    chooseAlgo();
    
    
    statsToCompute = vector<string>(1,"mean");
    
    if(isCaching){createExtractor();}
}

void AudioExtractor::createExtractor(){
    extr = new SimpleEssentiaExtractor(infos);
    extr->statsToCompute = statsToCompute;
    extr->initFile();
    extr->buildMe();
}
void AudioExtractor::chooseAlgo(){
    switch(type){
        case lowLevel:
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
            break;
        case MFCC:
            mapIt.name = "MFCC";
            mapIt.framecut.first = 2048;
            mapIt.framecut.second = 512;
            mapIt.inputName = "spectrum";
            mapIt.outputs.push_back("mfcc");
            infos.push_back(mapIt);
            break;
        case HPCP:
            mapIt.name = "SpectralPeaks";
            mapIt.framecut.first = 4096;
            mapIt.framecut.second = 512;
            mapIt.inputName = "spectrum";
            mapIt.outputs.push_back("Algo.HPCP");
            infos.push_back(mapIt);
            mapIt.outputs.clear();
            mapIt.name = "HPCP";
            mapIt.inputName = "Algo.HPCP";
            mapIt.outputs.push_back("hpcp");
            infos.push_back(mapIt);
            
            break;
            
        case MIXED:
            mapIt.name = "MFCC";
            mapIt.framecut.first = 2048;
            mapIt.framecut.second = 512;
            mapIt.inputName = "spectrum";
            mapIt.outputs.push_back("mfcc");
            infos.push_back(mapIt);
            mapIt.outputs.clear();
            
            mapIt.name = "SpectralPeaks";
            mapIt.inputName = "spectrum";
            mapIt.outputs.push_back("Algo.HPCP");
            infos.push_back(mapIt);
            mapIt.outputs.clear();
            
            mapIt.name = "HPCP";
            mapIt.framecut.first = 4096;
            mapIt.inputName = "Algo.HPCP";
            mapIt.outputs.push_back("hpcp");
            infos.push_back(mapIt);
            
            break;
            
            
            
            
    }
    
    
    
}
bool AudioExtractor::fillContainerBlock(const string & annotationPath){
    
    
    
    
    extr->setInput(containerBlock->parsedFile,"");
    extr->threadedFunction();
    
    vector<Real> onsets = extr->aggregatedPool.value<vector<Real>>("onsets");
    
    containerBlock->numElements = onsets.size();
    
    
    // Save to json to avoid to much heap memory alloc before loadFiles
   
    
    string destinationFile = getParsedFileCache(containerBlock->parsedFile);
    ofFile(destinationFile).create();
    ofLogVerbose("FileLoader") << "saving JSON at : " << destinationFile;
    extr->saveIt(destinationFile);
    containerBlock->song.audioPath = containerBlock->parsedFile;
    containerBlock->parsedFile = destinationFile;
    
    
    
    extr->outPool->clear();
    
    
}

string AudioExtractor::getParsedFileCache(const string & file){
    ofFile pFile(file);
    string destinationFile = ofFilePath::join(pFile.getEnclosingDirectory(),"Viza");
    destinationFile = ofFilePath::join(destinationFile, pFile.getBaseName() + ".json");
    return destinationFile;
}

int AudioExtractor::loadFile(){
    
    ofxJSONElement json;
  if(!json.open(containerBlock->parsedFile)){
    ofLogError("AudioExtractor") << "wrong analisis file : " << containerBlock->parsedFile;
    return 0;
  }
    
    vector<std::pair<float,float>> onsets;
    for(int i = 0; i <  json["slice"]["time"].size() ; i++){
        onsets.push_back(std::pair<float,float>(json["slice"]["time"][i][0].asFloat(),json["slice"]["time"][i][1].asFloat()));
    };
    
    
    
    int containerNum = containerBlock->containerIdx;
    
    
    float begin = 0;
    float end = 0;
    float songLength = onsets.back().second;
  
    for(int i = 0 ; i < containerBlock->numElements ; i++){
        
        
        begin = onsets[i].first;
        end = onsets[i].second;
        Container::containers[containerNum] = new Container(begin,end,containerNum,0);
        
        
        for(auto & v:json["values"].getMemberNames()){
            string name = v+".mean";
            ofxJSONElement value = json["values"][v]["mean"].get(i, 0);
            if(!value.isNumeric()){
                ofLogError("AudioLoader")<<" No feature found " << name;
                for (auto d:value){
                    ofLogError("AudioLoader") << "\t" << d.type();
                }
            }
            else{
                
                Container::containers[containerNum]->setAttribute(name,value.asFloat());
            }
        }
        
        //        Container::containers[containerNum]->setAttribute("length",end-begin);
        //        Container::containers[containerNum]->setAttribute("startTime",begin);
        //        Container::containers[containerNum]->setAttribute("relativeStartTime",songLength!=0?begin/(songLength):0);
        
        containerNum++;
    }
    if(std::any_of(json.getMemberNames().begin(),json.getMemberNames().end(),[](string s){return s=="metadata";}) &&
       std::any_of(json["metadata"].getMemberNames().begin(),json["metadata"].getMemberNames().end(),[](string s){return s=="audioPath";})
       ){
        string path = json["metadata"]["audioPath"].asString();
        if(std::filesystem::exists(path)){
            containerBlock->song.audioPath = path;
        }
        else{
            ofLogError("FileImporter") << "json has wrong audioPaths : " << path;
        }
    }
    //    for(map<string,vector<string> >::iterator itc = classMap.begin() ; itc !=classMap.end() ; ++itc){
    //        Container::containers[containerNum]->setClass(itc->first, itc->second[sliceNum]);
    //    }
    //    delete res;
    return 1;
    
    
    
    
    
};





vector<string> AudioExtractor::getAttributeNames(const string & path){
    auto map = extr->aggregatedPool.getRealPool();
    
    vector<string > res(map.size());
    int mapIdx = 0;
    for(auto a:map){
        res[mapIdx] = a.first.substr(7);
        mapIdx++;
    }
    
    return res;
    //    return mapIt.outputs;
    
    
}




