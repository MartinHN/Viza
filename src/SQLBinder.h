//
//  SQLBinder.h
//  ViZa
//
//  Created by martin hermant on 07/07/14.
//
//

#ifndef __ViZa__SQLBinder__
#define __ViZa__SQLBinder__


#include "SQLiteC++.h"
#include "ofMain.h"


class SQLBinder{
public:
    SQLBinder();
    
    void loadDB(string s = "defaultdb");
    
    
    
    SQLite::Database db;
};

#endif /* defined(__ViZa__SQLBinder__) */
