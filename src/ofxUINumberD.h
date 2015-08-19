//
//  ofxUINumberD.h
//  ViZa
//
//  Created by martin hermant on 19/08/15.
//
//

#ifndef __ViZa__ofxUINumberD__
#define __ViZa__ofxUINumberD__

#include "ofxUINumberDialer.h"


class ofxUINumberD : public ofxUINumberDialer{
public:
    ofxUINumberD( int _precision, string _name, int _size):
    ofxUINumberDialer(0.0f,1.0f,0.0f,_precision,_name,_size){};
    
    void setMin(float m){min = m;};
    void setMax(float m){min = m;};
    
};
#endif /* defined(__ViZa__ofxUINumberD__) */
