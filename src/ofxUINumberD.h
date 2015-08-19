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
    
    void setMin(float m){min = m;updatePrecision();};
    void setMax(float m){max = m;updatePrecision();};
    
    void updatePrecision(){
        float m = MAX(abs(min),abs(max));
        int digits = ( (m<1) ? 1 : int(1+log10(double(abs(m)))) );
        precision = (((numOfPrecisionZones-digits-1)>=0) ? (numOfPrecisionZones-digits-1):0);
    }


    
};
#endif /* defined(__ViZa__ofxUINumberD__) */
