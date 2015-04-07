//
//  FileImporter.cpp
//  ViZa
//
//  Created by martin hermant on 23/01/15.
//
//

#include "FileImporter.h"

#include "JsonLoader.h"
FileImporter* FileImporter::instance;

FileImporter::FileImporter(){

    BaseFileLoader::linkLoaders();
    
//    // Add capacity to the thread pool.
        Poco::ThreadPool::defaultPool().addCapacity(100);
//
//    // Limit the maximum number of tasks for shared thread pools.
        queue.setMaximumTasks(100);
    // Optionally listen for task custom notifications.
    ofAddListener(queue.onTaskFinished, this, &FileImporter::progress);
    hasLoaded = true;
    
}


void FileImporter::progress(const ofx::TaskQueueEventArgs& args){
    if(queue.getCount() == 0){
        onCompletion();
    }
    numDone++;
    progressPct = numDone*1.0/totalNumFile;
}

void FileImporter::crawlAnnotations(string annotationPath,string audioPath){
    
    hasLoaded = false;
    
    if(audioPath==""){
        audioPath = "/Users/mhermant/Documents/Work/Datasets/beatles/audio/wav";
    }
    if(annotationPath==""){
        annotationPath ="/Users/mhermant/Documents/Work/Dev/openFrameworks/apps/ViZa/bin/data/MaschineDrum_All_onset_200_4_f2048_h256/";
        //      annotationPath = "/Users/mhermant/Documents/Work/Datasets/IOWA/theremin.music.uiowa.edu/sound files/MIS/Piano_Other/";
    }
    
    ofDirectory ad =ofDirectory(audioPath);
    ad.allowExt("wav");
    ad.listDir();
    vector<ofFile> audioL = ad.getFiles();
    
    ad = ofDirectory(annotationPath);
    if(!ad.exists()){
        ofFileDialogResult f = ofSystemLoadDialog("analysisFiles",true);
        ad = ofDirectory(f.filePath);
        if(!ad.exists()){
            ofExit();
        }
    }
    
    for(BaseFileLoader::loaders_map_type::iterator it = BaseFileLoader::getMap()->begin() ; it!=BaseFileLoader::getMap()->end() ; ++it){
        ad.allowExt(it->first);
    }
    
    ad.listDir();
    vector<ofFile> segL = ad.getFiles();
    
    globalCount=0;
    
    
    getSubset(annotationPath+"meta/best.json");
    
    
    preCache(segL);
    

    totalNumFile = segL.size();
    numContainers = 0;
    for(std::vector<ofFile>::iterator p=segL.begin();p!= segL.end();++p){
        int contwatch = numContainers;
        int sliceNum = 0;
        BaseFileLoader * curLoader = BaseFileLoader::getMap()->at(p->getExtension())(ofToString(contwatch));
        curLoader->fillContainerBlock(p->path(),false);
        curLoader->containerBlock.startIdx = numContainers;
        queue.start(curLoader);
        numContainers+= curLoader->containerBlock.numSlices;
        sliceNum+=curLoader->containerBlock.numSlices;
        
        
        
        if( contwatch != numContainers){
            globalCount++;
        }
    }

    

    
}



void FileImporter::onCompletion(){
    Container::containers.resize(numContainers);
    Container::attributesCache.resize(numContainers*Container::attrSize);
    Container::CacheNormalized(numContainers);
    hasLoaded = true;
    
}


FileImporter * FileImporter::i(){
    if(instance == NULL) instance = new FileImporter();
    return instance;
}


void FileImporter::getSubset(string metapath){
    BaseFileLoader::subset.clear();
    vector < std::pair<float,string> > tmpsubset(0);
    
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
        if(tmpsubset.size()>numBest){
            startiit = tmpsubset.end() - numBest;
        }
        
        for( vector< std::pair<float,string> >::iterator iit = startiit ; iit < tmpsubset.end() ; ++iit ){
            BaseFileLoader::subset.push_back(iit->second);
            cout << iit->second << endl;
        }
        
        
    }
    
}

void FileImporter::preCache(const vector<ofFile>  & segL){
    ofFile refFile = segL[0];
    BaseFileLoader * curLoader = BaseFileLoader::getMap()->at(refFile.getExtension())("test");
    curLoader->fillContainerBlock(refFile.path(),true);
    sliceCacheSize = curLoader->containerBlock.numSlices;
    attributeNamesSize = curLoader->containerBlock.numAttributes + 3;
    curLoader->containerBlock.attributeNames.push_back("length");
    curLoader->containerBlock.attributeNames.push_back("startTime");
    curLoader->containerBlock.attributeNames.push_back("relativeStartTime");
    //preallorate huge number of segments for speed purposes (will be resized at the end)
    cout << Container::containers.size() << endl;
    Container::attributesCache.resize(Container::containers.size()+segL.size() * sliceCacheSize * attributeNamesSize);
    cout << attributeNamesSize << endl;
    cout << Container::attributesCache.size() << endl;
    Container::containers.resize(Container::containers.size()+segL.size() * sliceCacheSize);
    Container::preCacheAttr(curLoader->containerBlock.attributeNames);

    

    
}
bool FileImporter::savePosition(){
    for(map<string,vector<Container*> >::iterator it = Container::songs.begin() ;it != Container::songs.end() ; ++it){
        string outPath = Container::annotationPaths[it->first];
        ofxJSONElement json;
        ofFile f(outPath);
        ofDirectory d = f.getEnclosingDirectory();
        string finalPath = d.getAbsolutePath() +"/Viza/"+ f.getFileName();
        json.open(finalPath);
        for(int i = 0; i < it->second.size() ; i++){
            ofVec3f p = it->second[i]->getPos();
            json["Viza"].clear();
            for(int j = 0 ; j < 3 ; j++){
                json["Viza"].append(p[j]);
            }
        }

        json.save(finalPath);
    }
    
    
    
    
    
}
