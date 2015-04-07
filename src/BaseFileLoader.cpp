//
//  BaseFileLoader.cpp
//  ViZa
//
//  Created by martin hermant on 07/04/15.
//
//

#include "BaseFileLoader.h"


#include "JsonLoader.h"

vector<string> BaseFileLoader::subset(0);
BaseFileLoader::loaders_map_type * BaseFileLoader::loadersMap;





BaseFileLoader::BaseFileLoader(const std::string& name):Poco::Task(name){
    
}

BaseFileLoader::~BaseFileLoader(){
    
}

void BaseFileLoader::runTask(){

    loadFile();
}


void BaseFileLoader::linkLoaders(){
    getMap()->insert(std::make_pair("json", &createT<JsonLoader>));
}

BaseFileLoader * BaseFileLoader::getLoader(const string &extension,const string & name){
    
        loaders_map_type::iterator it = getMap()->find(extension);
        if(it == getMap()->end())
            return 0;
        return it->second(extension + " : " + name);
    
}

BaseFileLoader::loaders_map_type * BaseFileLoader::getMap() {
    // never delete'ed. (exist until program termination)
    // because we can't guarantee correct destruction order
    if(!loadersMap) { loadersMap = new loaders_map_type; }
    return loadersMap;
}
