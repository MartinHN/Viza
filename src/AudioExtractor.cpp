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

AudioExtractor::AudioExtractor(const std::string& name,bool isCaching):
BaseFileLoader(name,isCaching),
extr(nullptr){
    {
        
        ofScopedLock(staticMutex);
        if(!bEssentiaInited){
            bEssentiaInited =true;
            
        }
    }
    essentia::infoLevelActive = false;
    essentia::warningLevelActive = false;
    essentia::errorLevelActive = false;
    maxAnalysingThread = 4 ;
    maxImportingThread = 10 ;
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
    
    if(isCaching){createExtractor();}
}

void AudioExtractor::createExtractor(){
    extr = new SimpleEssentiaExtractor(infos);
    extr->statsToCompute = statsToCompute;
    extr->initFile();
    extr->buildMe();
}
bool AudioExtractor::fillContainerBlock(const string annotationPath){
    
    extr->setInput(containerBlock->parsedFile,"");
    extr->threadedFunction();
    vector<Real> onsets = extr->aggregatedPool.value<vector<Real>>("onsets");
    containerBlock->numElements = onsets.size();
    containerBlock->song.length = extr->outPool.value<Real>("metadata.duration");
    containerBlock->song.audioPath = annotationPath;
    containerBlock->song.numSlices = onsets.size();
    containerBlock->song.name = ofFile(annotationPath).getBaseName();
    
    
    if(onsets.size()==0){
        map<string,Real> tmp =  extr->aggregatedPool.getSingleRealPool();
        for(auto t:tmp){
            containerBlock->data[t.first]  = vector<Real> (1,t.second);
        }
    }
    else{
        containerBlock->data = extr->aggregatedPool.getRealPool();
        map<string,vector<Real> >  r =extr->aggregatedPool.getSingleVectorRealPool();
        vector<float> onsets = r["onsets"];
        containerBlock->data["onsets"] = onsets;
    }
    
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
    for(int i = 0 ; i < containerBlock->numElements ; i++){
        
        Container::containers[containerNum] = new Container(begin,end,containerNum,0);
        begin = containerBlock->data["onsets"][i];
        if(i== containerBlock->data["onsets"].size() - 1)
            end = containerBlock->song.length;
        else
            end = containerBlock->data["onsets"][i+1];
        
        
        
        for(auto & v:mapIt.outputs){
            string name = "values."+v+".mean";
            if(containerBlock->data.count(name)==0){
                ofLogError("AudioLoader")<<" No feature found " << name;
                //            for (auto d:containerBlock->data){
                //                ofLogError("AudioLoader") << "\t" << d.first;
                //            }
            }
            
            Container::containers[containerNum]->setAttribute(v,containerBlock->data.at(name)[i]);
        }
        
        
        // Add Meta Info
        Container::containers[containerNum]->setAttribute("length",end-begin);
        Container::containers[containerNum]->setAttribute("startTime",begin);
        Container::containers[containerNum]->setAttribute("relativeStartTime",containerBlock->song.length!=0?begin/(containerBlock->song.length):0);
        containerNum++;
    }
    
    
    //    for(map<string,vector<string> >::iterator itc = classMap.begin() ; itc !=classMap.end() ; ++itc){
    //        Container::containers[containerNum]->setClass(itc->first, itc->second[sliceNum]);
    //    }
    //    delete res;
    return 1;
    
    
    
    
    
};






vector<string> AudioExtractor::getAttributeNames(const string & path){
    return mapIt.outputs;
    
    
}




