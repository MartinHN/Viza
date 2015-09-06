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


FileImporter* FileImporter::instance;

FileImporter::FileImporter(){
    
    
    BaseFileLoader::linkLoaders();
    int numThread = 1;
    //    // Add capacity to the thread pool.
    Poco::ThreadPool::defaultPool().addCapacity(MAX(0,numThread-Poco::ThreadPool::defaultPool().capacity()));
    //
    //    // Limit the maximum number of tasks for shared thread pools.
    queue.setMaximumTasks(numThread);
    
    
    
    queue.registerTaskProgressEvents(this);
    
    hasLoaded = true;
    progressPct = 0;
    
}


void FileImporter::onTaskFinished(const ofx::TaskQueueEventArgs& args){
    

    {
        ofScopedLock sl(mutex);
        taskProgress.erase(args.getTaskId());
        numDone++;
        updateProgress();
    }
        if( numDone == totalNumFile){
            
            if(!isCaching){
                ofLogNotice("FileImporter") << "completed "  << numDone << " files";
                onCompletion();}
            else{
                ofLogNotice("FileImporter") << "caching completed "  << numDone << " files";
            }
        }
        
    
 
    
    
}

void FileImporter::onTaskQueued(const ofx::TaskQueueEventArgs& args){
    taskProgress[args.getTaskId()] = 0;
}


void FileImporter::onTaskStarted(const ofx::TaskQueueEventArgs& args){}
void FileImporter::onTaskCancelled(const ofx::TaskQueueEventArgs& args){taskProgress.erase(args.getTaskId());ofLogError("FileImporter","task cancelled");}
void FileImporter::onTaskFailed(const ofx::TaskFailedEventArgs& args){
    taskProgress.erase(args.getTaskId());ofLogError("FileImporter")<<"task failed : "<< args.getException().message();}
void FileImporter::onTaskProgress(const ofx::TaskProgressEventArgs& args){
    ofScopedLock sl(mutex);
    taskProgress[args.getTaskId()] = args.getProgress();
    updateProgress();
    
}

void FileImporter::updateProgress(){
    //    ofScopedLock sl(mutex);
    float locProgress=0;
    if(taskProgress.size()){
        for(TaskProgress::iterator it = taskProgress.begin() ; it != taskProgress.end() ; ++it){
            locProgress+=it->second;
        }
        locProgress/= taskProgress.size();
    }
    progressPct = (numDone + locProgress)*1.0/totalNumFile;
    //    progressPct = (numDone )*1.0/totalNumFile;
}


void FileImporter::crawlAnnotations(string annotationPath,string audioPath){
    hasLoaded = false;
    annotationfolderPath = ofFilePath::getPathForDirectory(annotationPath);
    if(audioPath=="")audiofolderPath = findAudioPath(annotationPath);
    else audiofolderPath = audioPath;
    ofDirectory ad = ofDirectory(annotationfolderPath);
    
    
    BaseFileLoader::annotationFolderPath = annotationfolderPath;
    BaseFileLoader::audioFolderPath = audiofolderPath;
    
    if(!ad.exists()){
        ofFileDialogResult f = ofSystemLoadDialog("analysisFiles",true);
        ad = ofDirectory(f.filePath);
        if(!ad.exists() || !f.bSuccess){
            ofExit();
        }
    }
    
    vector<string> extensions = BaseFileLoader::getAllowedExtensions();
    vector<filesystem::path> Paths = FileUtils::getFilePathsWithExt(annotationfolderPath,extensions);
    if(Paths.size() == 0 ){
        ofLogError ("FileImporter")<<"no valid extentions found";
        ofExit();
        return;
    }
    curLoader = BaseFileLoader::getMap()->at(Paths[0].extension().string())("test");
    if(!curLoader->hasCachedInfo() ||
       ofSystemTextBoxDialog("would you like to updated cached information\
                             about this dataset?\
                             (fill anything below to re build cache","")!=""
       ){
        isCaching = true;
    }
    
    
    this->startThread();
}

void FileImporter::threadedFunction(){
    
    isCaching = true;
    int cacheNum = 0;
    int containerIndex = 0;
    numDone=0;

    vector<filesystem::path> segL = FileUtils::getFilePathsWithExt(annotationfolderPath, curLoader->extensions);
    totalNumFile= segL.size();
    queue.cancelAll();
    infos.resize(segL.size());
    bool init = true;
    {

        for(std::vector<filesystem::path>::iterator p=segL.begin();p!= segL.end();++p){
            BaseFileLoader * curLoader = BaseFileLoader::getMap()->at(p->extension().string())(ofToString(cacheNum));
            curLoader->isCaching = true;
            // indicate context for task
            curLoader->containerBlock = &infos[cacheNum];
            curLoader->containerBlock->parsedFile = p->string();
            curLoader->containerBlock->songIdx = cacheNum;
            curLoader->containerBlock->containerIdx = cacheNum;
            if(init){
                BaseFileLoader::globalInfo.attributeNames = curLoader->getAttributeNames(p->string());
                init = false;
            }
            
            queue.start(curLoader);
            cacheNum ++;

            
        }
    }
    
    //lock to activeQueue
    while(queue.getCount() || queue.getActiveCount()){};
    taskProgress.clear();
    queue.joinAll();
    
    //    curLoader->endCaching();
    isCaching = false;
    
    
    
    //    vector<filesystem::path> segL = FileUtils::getFilePathsWithExt(annotationfolderPath, curLoader->extensions);
    
    int globalCount=0;
    
    
    
    
    //    getSubset(annotationfolderPath+"Viza/best.json");
    
    
    preCache(infos);
    
    
    
    
    int numContainers = 0;
    numSong = 0;
    dbgTime = ofGetElapsedTimef();
    queue.cancelAll();
    BaseFileLoader::audioFolderPath = audiofolderPath;
    numDone = 0;
    totalNumFile = 0;
    {
        ofScopedLock sl(mutex);
        for(std::vector<filesystem::path>::iterator p=segL.begin();p!= segL.end();++p){
            int contwatch = numContainers;
            BaseFileLoader * curLoader = BaseFileLoader::getMap()->at(p->extension().string())(ofToString(contwatch));
            
            curLoader->containerBlock = &infos[numSong];
            

            numContainers+= curLoader->containerBlock->numElements;
            
            if( contwatch != numContainers){
                totalNumFile++;
                queue.start(curLoader);
                globalCount++;
                numSong++;
            }
            else{
                
                ofLogWarning("FileImporter") << "nothing to add for file " << p->string();
            }
        }
    }

    ofLogNotice("FileImporter","importing "+ofToString(globalCount)+" annotation files");
    
//    while(queue.getCount()){};
    
    
    
}



void FileImporter::onCompletion(){
    
    
    
    ofEventArgs dumb = ofEventArgs();
    ofNotifyEvent(ofEvents().update,dumb);
    queue.joinAll();
    ofLogWarning("FileImporter",ofToString(Container::numContainer) + " container created in : " + ofToString(ofGetElapsedTimef() - dbgTime ) + " seconds");
    Container::numAttr = Container::numContainer*Container::attrSize;
    Container::containers.resize(Container::numContainer);
    
    Container::attributesCache = (float*)realloc(Container::attributesCache,sizeof(float)*Container::numAttr);
    Container::CacheNormalized(Container::numContainer);
    hasLoaded = true;
    
    
    
    ofEvents().enable();
    Physics::resizeVBO();
    GUI::i()->setup();
    Container::registerListener();
    
    //    for(vector< vector<unsigned int> >::iterator it = Container::songsContainers.begin() ; it != Container::songsContainers.end() ; ++it ){
    //        for(int i = 0 ; i <POLYPHONY ; i++){
    //            //           AudioPlayer::Load(*it->second[i], true);
    //        }
    //    }
    
}


FileImporter * FileImporter::i(){
    if(instance == NULL) instance = new FileImporter();
    return instance;
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

void FileImporter::preCache( vector<BaseFileLoader::ContainerBlockInfo> & v){
    
    BaseFileLoader::globalInfo.totalSong = v.size();
    
    // fills GlobalInfo
//    if(curLoader->hasCachedInfo()){
//        curLoader->getCachedInfo(annotationfolderPath);
//    }
//    else{
    
        BaseFileLoader::globalInfo.totalContainers = 0;
    for(int i = 0 ; i < v.size() ; i ++){
            BaseFileLoader::globalInfo.totalContainers += v[i].numElements;
        }
        
        
//    }
    
    int totalContainers = BaseFileLoader::globalInfo.totalContainers;
    
    if(!BaseFileLoader::globalInfo.hasVizaMeta){
        BaseFileLoader::globalInfo.attributeNames.push_back("length");
        BaseFileLoader::globalInfo.attributeNames.push_back("startTime");
        BaseFileLoader::globalInfo.attributeNames.push_back("relativeStartTime");
    }
    int attributeNamesSize = BaseFileLoader::globalInfo.attributeNames.size();
    //preallorate huge number of segments for speed purposes (will be resized at the end)
    ofLogWarning("FileImporter","allocating :"+ofToString(totalContainers) + " containers for " + ofToString(attributeNamesSize) + " attributes");
    if(Container::attributesCache!=NULL)free(Container::attributesCache);
    Container::numAttr = totalContainers*attributeNamesSize;
    Container::attributesCache = (float*) malloc(Container::numAttr*sizeof(float));
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

void FileImporter::saveProto(){
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

#endif

string FileImporter::findAudioPath(const string & p){
    string res = "";
    string audioInfoPath = ofFilePath::join(p, "Viza/audioInfo.json");
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



void FileImporter::loadAnalysisFiles(string segpath,string audiopath){
    
    ofEvents().disable();
    ofEvents().update.enable();
    ofEvents().draw.enable();
    AudioPlayer::UnloadAll();
    Container::clearAll();
    i()->crawlAnnotations(segpath,audiopath);
    Physics::clearAll();
    
    
}





