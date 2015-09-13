//
//  FileImporter.cpp
//  ViZa
//
//  Created by martin hermant on 23/01/15.
//
//

#include "FileImporter.h"



#include "ofxJSONElement.h"
#include "GUI.h"
#define MAX_NUMTHREADS 6

FileImporter* FileImporter::instance;

FileImporter::FileImporter(){
    
    BaseFileLoader::linkLoaders();
    
    hasLoaded = true;
    progressPct = 0;
    
    ofAddListener(importEvent,this,&FileImporter::eventRecieved);
    ofAddListener(ofEvents().update, this, &FileImporter::update);
    needUpdate = false;
}


void FileImporter::update(ofEventArgs & a){
    if(needUpdate){
        onCompletion();
        needUpdate = false;
    }
    
    
}


void FileImporter::eventRecieved(importEventArg & a){}
bool FileImporter::crawlAnnotations(string annotationPath,string audioPath){
    if(isThreadRunning()){ofLogError("FileImporter") << "already importing";return false;}
    
    AudioPlayer::UnloadAll();
    Container::clearAll();
    
    hasLoaded = false;
    annotationfolderPath = ofFilePath::getPathForDirectory(annotationPath);
    
    if(audioPath==""){
        if(!FileUtils::seemsAudioFolder(annotationPath)){audiofolderPath = findAudioPath(annotationPath);}
        else{audiofolderPath  = annotationPath;}
    }
    else {audiofolderPath = audioPath;}
    ofDirectory ad = ofDirectory(annotationfolderPath);
    
    
    BaseFileLoader::annotationFolderPath = annotationfolderPath;
    BaseFileLoader::audioFolderPath = audiofolderPath;
    
    if(!ad.exists()){
        ofFileDialogResult f = ofSystemLoadDialog("analysisFiles",true);
        ad = ofDirectory(f.filePath);
        if(!ad.exists() || !f.bSuccess){
            return false;
        }
    }
    
    
    vector<string> extensions = BaseFileLoader::getAllowedExtensions();
    vector<filesystem::path> Paths = FileUtils::getFilePathsWithExt(annotationfolderPath,extensions);
    if(Paths.size() == 0 ){ofLogError ("FileImporter")<<"no valid extentions found";return false;}
    curLoader = BaseFileLoader::getMap()->at(Paths[0].extension().string())("test",false);
    isCaching = !curLoader->hasGlobalInfo() ||
    ofSystemTextBoxDialog("would you like to updated cached information about this dataset?\
                          (fill anything below to re build cache)","")!="";
    
    
    this->startThread();
    return true;
}

void FileImporter::threadedFunction(){
    
    progressPct=0;
    int cacheNum = 0;
    int containerIndex = 0;

    dbgTime = ofGetElapsedTimef();
    
    bool imported = false;
    
    vector<string> extensions = BaseFileLoader::getAllowedExtensions();
    vector<filesystem::path> segL = FileUtils::getFilePathsWithExt(annotationfolderPath,extensions);
    if(segL.size() == 0 ){ofLogError ("FileImporter")<<"no valid extentions found";return false;}
    curLoader = BaseFileLoader::getMap()->at(segL[0].extension().string())("test",false);
    segL = FileUtils::getFilePathsWithExt(annotationfolderPath, curLoader->extensions);

    totalNumFile= segL.size();
    
    infos.resize(segL.size());
    
    
    
    if(isCaching) {
        
        
        // Cache Info to 
        threadpool<BaseFileLoader,BaseFileLoader::ContainerBlockInfo> queue(&progressPct);
        int qSize = MIN(curLoader->maxAnalysingThread,infos.size());
        for(int i = 0 ; i < qSize;i++){
            queue.addWorker(BaseFileLoader::getMap()->at(curLoader->extensions[0])(ofToString(i),true));
        }
        //        ofScopedLock sl(mutex);
        int cIIdx =0;
        for(std::vector<filesystem::path>::iterator p=segL.begin();p!= segL.end();++p){
            
            BaseFileLoader::ContainerBlockInfo * cI = new BaseFileLoader::ContainerBlockInfo();
            infos[cIIdx]=cI;
            // indicate context for task
            cI->parsedFile = p->string();
            cI->songIdx = cacheNum;

            static bool init = true;
            if(init)  BaseFileLoader::globalInfo.attributeNames = curLoader->getAttributeNames(p->string());
            init = false;
            queue.addTask(cI);
            cacheNum ++;
            cIIdx++;
            
            
        }
        //wait cache
        queue.joinAll();
        // update general infos
        updateGlobalInfo();
        BaseFileLoader::saveGlobalInfo();
    }
    else {
        BaseFileLoader::setGlobalInfo();
            int cIIdx =0;
        for(std::vector<filesystem::path>::iterator p=segL.begin();p!= segL.end();++p){
            infos[cIIdx] = new BaseFileLoader::ContainerBlockInfo();
            cIIdx++;
        }
    }
    int cIIdx =0;
    int totalContainer = 0;
    for(std::vector<filesystem::path>::iterator p=segL.begin();p!= segL.end();++p){
        
        BaseFileLoader::ContainerBlockInfo * cI=infos[cIIdx];
        // indicate context for task
        cI->parsedFile = curLoader->getParsedFileCache(p->string());
        cI->songIdx = cIIdx;
        cI->numElements = BaseFileLoader::globalInfo.containerSizes[cIIdx];
        cI->containerIdx = totalContainer;
        totalContainer+= cI->numElements;
        cIIdx++;
        cI->song.numSlices = cI->numElements;
        cI->song.idx = cI->containerIdx;
        cI->song.annotationPath = p->string();
        cI->song.name = ofFile(p->string()).getBaseName();
        
    }
    
    
    isCaching = false;
    
    int globalCount=0;
    
    
    
    preCache();
    
    
    
    
    int numContainers = 0;
    numSong = 0;
    BaseFileLoader::audioFolderPath = audiofolderPath;

    totalNumFile = 0;
    {
        threadpool<BaseFileLoader,BaseFileLoader::ContainerBlockInfo> queue(&progressPct);
        int qSize = MIN(curLoader->maxImportingThread,infos.size());
        for(int i = 0 ; i < qSize;i++){
            queue.addWorker(BaseFileLoader::getMap()->at(segL[0].extension().string())(ofToString(i),false));
        }
        //        ofScopedLock sl(mutex);
        int cIIdx = 0;
        for(std::vector<filesystem::path>::iterator p=segL.begin();p!= segL.end();++p){
            int contwatch = numContainers;
            BaseFileLoader::ContainerBlockInfo * cI = infos[cIIdx];
            
            numContainers+= cI->numElements;
            
            if( cI->numElements!=0){
                totalNumFile++;
                queue.addTask(cI);
                globalCount++;
                numSong++;
            }
            else{
                
                ofLogWarning("FileImporter") << "nothing to add for file " << p->string();
            }
            cIIdx++;
        }
    }
    
    ofLogNotice("FileImporter","imported "+ofToString(globalCount)+" annotation files");
    needUpdate = true;
    for(auto in:infos){
        delete in;
    }
    infos.clear();
    
    //
    //     importEventArg dumb ;
    //    ofNotifyEvent(importEvent,dumb);
    
    
}

void FileImporter::updateGlobalInfo(){
    BaseFileLoader::globalInfo.hasVizaMeta = std::any_of(BaseFileLoader::globalInfo.attributeNames.begin(),
                                                         BaseFileLoader::globalInfo.attributeNames.end(),
                                                         [](string s){return (s=="length"||s=="relativeStartTime"||s=="startTime");});
    if(!BaseFileLoader::globalInfo.hasVizaMeta){
        BaseFileLoader::globalInfo.attributeNames.push_back("length");
        BaseFileLoader::globalInfo.attributeNames.push_back("startTime");
        BaseFileLoader::globalInfo.attributeNames.push_back("relativeStartTime");
        
    }
    
    BaseFileLoader::globalInfo.totalSong = 0;
    BaseFileLoader::globalInfo.totalContainers = 0;
    
    
    BaseFileLoader::globalInfo.containerSizes.clear();
    BaseFileLoader::globalInfo.containerSizes.reserve(infos.size());
    for(auto cI:infos){
        BaseFileLoader::globalInfo.totalContainers += cI->numElements;
        BaseFileLoader::globalInfo.totalSong ++;
        BaseFileLoader::globalInfo.containerSizes.push_back(cI->numElements);

    }
    
}

void FileImporter::onCompletion(){
    
    
    ofLogWarning("FileImporter",ofToString(Container::numContainer) + " container created in : " + ofToString(ofGetElapsedTimef() - dbgTime ) + " seconds");
    Container::totalAttr = Container::numContainer*Container::attrSize;
    Container::containers.resize(Container::numContainer);
    
    Container::attributesCache.conservativeResize(Container::attrSize,Container::numContainer);
    Container::CacheNormalized(Container::numContainer);
    hasLoaded = true;
    
    
    
    ofEvents().enable();
    Physics::resizeVBO();
    GUI::i()->setup();
    Container::registerListener();
    
}


FileImporter * FileImporter::i(){
    if(instance == NULL) instance = new FileImporter();
    return instance;
}



void FileImporter::preCache( ){
    
    
    
    Container::attrSize = BaseFileLoader::globalInfo.attributeNames.size();
    Container::numContainer = BaseFileLoader::globalInfo.totalContainers;
    
    int totalContainers = BaseFileLoader::globalInfo.totalContainers;
    
    if(!BaseFileLoader::globalInfo.hasVizaMeta){
        BaseFileLoader::globalInfo.attributeNames.push_back("length");
        BaseFileLoader::globalInfo.attributeNames.push_back("startTime");
        BaseFileLoader::globalInfo.attributeNames.push_back("relativeStartTime");
    }
    int attributeNamesSize = BaseFileLoader::globalInfo.attributeNames.size();
    //preallorate huge number of segments for speed purposes (will be resized at the end)
    ofLogWarning("FileImporter","allocating :"+ofToString(totalContainers) + " containers for " + ofToString(attributeNamesSize) + " attributes");
    
    Container::totalAttr = totalContainers*attributeNamesSize;
    Container::attributesCache.resize(Container::attrSize,Container::numContainer);
    Container::attributesCache.setZero();
    
    Container::containers.resize(totalContainers);
    ofLogNotice("FileImporter","totalSize meta:"+ofToString(sizeof(Container::containers))+ " data : "+ofToString(sizeof(Container::attributesCache)));
    Container::preCacheAttr(BaseFileLoader::globalInfo.attributeNames);
    ofLogNotice("FileImporter","allocating :"+ofToString(BaseFileLoader::globalInfo.totalSong) + " songs " );
    Container::songMeta.resize(BaseFileLoader::globalInfo.totalSong);
    Container::songsContainers.resize(BaseFileLoader::globalInfo.totalSong);
    
    
}
bool FileImporter::savePosition(){
    int idx = 0;
    
    ofxJSONElement json;
    for(vector< vector<unsigned int> >::iterator it = Container::songsContainers.begin() ;it != Container::songsContainers.end() ; ++it){
        ofVec3f pos = Physics::vs[ it->at(0) ];
        for (int i = 0; i < 2 ; i++){
            json[Container::songMeta[idx].name] .append(pos[i]);
        }
        idx ++;
    }
    
    
    ofFileDialogResult fileRes = ofSystemSaveDialog("out.json","output File");
    if(fileRes.bSuccess){
        json.save(fileRes.filePath);
    }
}


#ifdef PROTOBUF_SUPPORT

void FileImporter::save(){
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    
    string destinationPath = annotationfolderPath+"_Viza/";
    
    {
        // save global info
        
        string destinationFile = destinationPath+"globalViza.viza";
        ofFile fout;
        fout.open(destinationFile,ofFile::WriteOnly);
        Viza::GlobalList global;
        
        for(Container::ClassMapStruct::iterator it = Container::classeMap.begin() ; it!= Container::classeMap.end() ; ++it){
            string className = it->first;
            Viza::Class * cl = global.add_classes();
            for(Container::ClassValueStruct::iterator itt = it->second.begin() ; itt!= it->second.end() ; ++itt){
                cl->add_values(itt->first);
            }
            cl->set_name(className);
        }
        
        for(vector<string>::iterator attrit = Container::attributeNames.begin() ; attrit != Container::attributeNames.end() ; ++attrit){
            global.add_attributesnames(*attrit);
        }
        
        global.set_numcontainer(Container::containers.size());
        
        for(vector< Container::SongMeta >::iterator orit = Container::songMeta.begin() ; orit!= Container::songMeta.end() ;orit++){
            Viza::Song * song = global.add_songs();
            
            
            song->set_name(orit->name);
            song->set_audiopath(orit->audioPath);
            song->set_length(orit->length);
            song->set_numslice(orit->numSlices);
            
        }
        
        if (!global.SerializeToOstream(&fout)){
            ofLogError("FileImporter", "Failed to write Global Protobuf at " +destinationFile);
            
        }
    }
    
    
    // save files;
    int curSongIdx = 0;
    for(vector< Container::SongMeta >::iterator orit = Container::songMeta.begin() ; orit!= Container::songMeta.end() ;orit++){
        string destinationFile = destinationPath+orit->name+".vizad";
        Viza::ContainerList protobuf;
        ofFile fout ;
        fout.open(destinationFile,ofFile::WriteOnly);
        
        
        for(vector<unsigned int >::iterator cit = Container::songsContainers[orit->idx].begin() ; cit!= Container::songsContainers[orit->idx].end() ;cit++){
            
            Viza::Container* cont = protobuf.add_containers();
            Container * locContainer = Container::containers[*cit];
            cont->set_sliceidx(locContainer->sliceIdx);
            cont->set_slicebegin(locContainer->begin);
            cont->set_sliceend(locContainer->end);
            
            
            
            for(int num = 0 ; num <Container::attributeNames.size();++num){
                cont->add_attributevalues(locContainer->getAttributes(num));
            }
            unsigned int locIdx = locContainer->globalIdx;
            
            for(Container::ClassMapStruct::iterator it = Container::classeMap.begin() ; it!= Container::classeMap.end() ; ++it){
                
                bool foundClass = false;
                unsigned int classNum = 0;
                
                for(Container::ClassValueStruct::iterator itt = it->second.begin() ; !foundClass,itt!=it->second.end() ; ++itt){
                    
                    for(vector<unsigned int >::iterator ittt = itt->second.begin() ; !foundClass,ittt!=itt->second.end() ; ++ittt){
                        
                        if(*ittt == locIdx){
                            
                            cont->add_classes(classNum);
                            foundClass = true;
                            break;
                        }
                        
                    }
                    if(foundClass)break;
                    
                    
                    classNum++;
                    
                }
                if(!foundClass){
                    ofLogError("FileImporter", "not found class while saving protobuf");
                }
            }
            
        }
        
        
        
        protobuf.set_songidx(curSongIdx);
        
        
        if (!protobuf.SerializeToOstream(&fout)){
            ofLogError("FileImporter", "Failed to write file Protobuf :"+destinationFile );
            
        }
        fout.close();
        curSongIdx++;
    }
    
    ofLogWarning("FileImporter","saved protobufs in "+ destinationPath);
    
}

#else

void FileImporter::save(){
    
    
    string destinationPath = annotationfolderPath+"_Viza/";
    
    
    // save global info
    ofxJSONElement json;
    
    
    // save files;
    int curSongIdx = 0;
    for(vector< Container::SongMeta >::iterator orit = Container::songMeta.begin() ; orit!= Container::songMeta.end() ;orit++){
        string destinationFile = destinationPath+orit->name+".json";
        // save global info
        ofxJSONElement json;
        
        int sliceIdx = 0;
        Json::Value times;
        times.resize(orit->numSlices);
        for(int num = 0 ; num <Container::attributeNames.size();++num){
            json["values"][Container::attributeNames[num]].resize(orit->numSlices);
        }
        json["values"]["x"].resize(orit->numSlices);
        json["values"]["y"].resize(orit->numSlices);
        json["values"]["z"].resize(orit->numSlices);
        
        for(vector<unsigned int >::iterator cit = Container::songsContainers[orit->idx].begin() ; cit!= Container::songsContainers[orit->idx].end() ;cit++){
            
            
            
            
            Container * cont = Container::containers[*cit];
            
            Json::Value slice;
            times[sliceIdx].resize(2);
            times[sliceIdx][0] = cont->begin;
            times[sliceIdx][1] = cont->end;
            for(int num = 0 ; num <Container::attributeNames.size();++num){
                json["values"][Container::attributeNames[num]][sliceIdx] = cont->getAttribute(num);
            }
            
            json["values"]["x"][sliceIdx] = Physics::vs[*cit].x;
            json["values"]["y"][sliceIdx] = Physics::vs[*cit].y;
            json["values"]["z"][sliceIdx] = Physics::vs[*cit].z;
            
            
            unsigned int locIdx = cont->globalIdx;
            
            for(Container::ClassMapStruct::iterator it = Container::classeMap.begin() ; it!= Container::classeMap.end() ; ++it){
                
                bool foundClass = false;
                unsigned int classNum = 0;
                
                for(Container::ClassValueStruct::iterator itt = it->second.begin() ; !foundClass,itt!=it->second.end() ; ++itt){
                    
                    for(vector<unsigned int >::iterator ittt = itt->second.begin() ; !foundClass,ittt!=itt->second.end() ; ++ittt){
                        
                        if(*ittt == locIdx){
                            
                            //                            cont->add_classes(classNum);
                            foundClass = true;
                            break;
                        }
                        
                    }
                    if(foundClass)break;
                    
                    
                    classNum++;
                    
                }
                if(!foundClass){
                    ofLogError("FileImporter", "not found class while saving protobuf");
                }
            }
            sliceIdx++;
        }
        json["slice"]["time"] = times;
        
        
        json.save(destinationFile);
        curSongIdx++;
    }
    
    ofLogWarning("FileImporter","saved json in "+ destinationPath);
    
}

#endif

string FileImporter::findAudioPath(const string & p){
    string res = "";
    string audioInfoPath = ofFilePath::join(p, "VizaMeta/audioInfo.json");
    ofFile f(audioInfoPath);
    
    
    if(f.exists()){
        ofxJSONElement json;
        json.open(f.path());
        res = json.get("audioDirectory","").asString();
        
    }
    else{
        ofFileDialogResult resf = ofSystemLoadDialog("Choose audio folder",true);
        if(resf.bSuccess){
            res = resf.filePath;
            ofxJSONElement json;
            
            json["audioDirectory"] = res;
            json.save(audioInfoPath);
            
            
        }
        
    }
    
    
    ofLogVerbose("FileImporter","audio directory set to : " + res);
    
    return res;
}



bool FileImporter::loadAnalysisFiles(string segpath,string audiopath){
    
    ofEvents().disable();
    ofEvents().update.enable();
    ofEvents().draw.enable();
    
    if(i()->crawlAnnotations(segpath,audiopath)){
        Physics::clearAll();
        return true;
    }
    else return false;
    
    
}




void FileImporter::getSubset(string metapath){
    
    vector < std::pair<float,string> > tmpsubset(0);
    BaseFileLoader::attrSubset.clear();
    
    
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
        ofLogWarning("FileImporter","getting subset :");
        for( vector< std::pair<float,string> >::iterator iit = startiit ; iit < tmpsubset.end() ; ++iit ){
            BaseFileLoader::attrSubset.push_back(iit->second);
            
            ofLogWarning("FileImporter", "\t" +iit->second );
        }
        
        
    }
    
}




