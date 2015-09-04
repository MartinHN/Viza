//
//  SimpleExtractor.cpp
//  ViZa
//
//  Created by martin hermant on 03/09/15.
//
//

#include "SimpleExtractor.h"

REGISTER_EXTRACTOR(SimpleExtractor);


void  SimpleExtractor::init() {
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
    
    
};

void SimpleExtractor::setInput(fileInputInfo &fileIO){

    extr->setInput(fileIO.audioPath,fileIO.outputPath,fileIO.classes);
}

void SimpleExtractor::compute(){

    extr->threadedFunction();
    
}