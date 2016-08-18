//
//  DragOut.cpp
//  ViZa
//
//  Created by Martin Hermant on 06/09/2015.
//
//


#ifdef __APPLE__
#include "DragOut.h"

#ifdef __OBJC__

#include "DragOut.mm"
#endif

#include "ofMain.h"

bool DragOut::performExternalDragDrop (const std::vector<std::string>& files,string tmpFolder,const std::vector<float>& starts,const std::vector<float>& ends, void * window,float x, float y){

    tmpFolder =ofDirectory(tmpFolder).getAbsolutePath();
    if(starts.size()!= ends.size() ){
        ofLogError("DragOut") << "size of start and ends mismatch";
        return false;
    }
    vector<string> newFiles(0);
    for(int f = 0 ; f < files.size() ; f++){
        
        essentia::standard::Algorithm * inWav = essentia::standard::AlgorithmFactory::create("AudioLoader","filename",files[f]);
        
        
        std::vector<essentia::StereoSample> audio;//44100*60*30);
        float sR;
        int numChan;
        string md5;
        int bR;
        string codec;
        
        inWav->output("audio").set(audio);
        inWav->output("sampleRate").set(sR);
        inWav->output("numberChannels").set(numChan);
        inWav->output("md5").set(md5);
        inWav->output("bit_rate").set(bR);
        inWav->output("codec").set(codec);

        
        inWav->compute();
        
        delete inWav;
        
        float sampleRate = 44100.0;
        for(int i = 0 ; i < starts.size() ; i++){
            
            int start = starts[i]*sR;
            int end = ends[i]*sR ;
            cout << start << " / " << end << endl;
            vector<essentia::StereoSample> slice(audio.begin()+start,audio.begin()+end);
            
            string fileOut = ofFilePath::join(tmpFolder ,ofFile(files[f]).getBaseName()) + "_"+std::to_string(i);
            fileOut+= ".wav";
            int j = i;
            while(filesystem::exists(filesystem::path(fileOut))){
                j++;
                fileOut = ofFilePath::join(tmpFolder ,ofFile(files[f]).getBaseName()) + "_"+std::to_string(j) + ".wav";
            }

            
            essentia::standard::Algorithm * outWav =
            essentia::standard::AlgorithmFactory::create("AudioWriter","filename",fileOut);
            outWav->input("audio").set(slice);
            outWav->compute();
            newFiles.push_back(fileOut);
            delete outWav;
        }
        
    }
    
    performExternalDragDropOfFiles(newFiles, window, x, y);
    
    
    
    
    
    



}


#endif  // targetOSX