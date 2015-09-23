//
//  AudioPlayer.h
//  ViZa
//
//  Created by martin hermant on 09/10/14.
//
//

#ifndef __ViZa__AudioPlayer__
#define __ViZa__AudioPlayer__

#include <iostream>
#include "ofMain.h"

#include "Timed.h"
#include "Container.h"
#include "fmod.h"

class Container;


#define POLYPHONY 10

typedef struct audioUID{
    string name;
    int idx;
    
    bool  operator==(const audioUID &o) const{
        return idx == o.idx && name == o.name;
    }
    
    bool  operator<(const audioUID &o) const{
        return idx < o.idx ;
    }
    
    const string toString() const{
        return  name + "\n" + ofToString(idx) ;
    }


    friend class AudioPlayer ;
    
}audioUID;




class AudioPlayer{
public:
    AudioPlayer();
    
    static AudioPlayer * instance(){
        if(inst==NULL){inst = new AudioPlayer();
//            ofAddListener(ofFmodSoundPlayer::audioEvent,inst,&AudioPlayer::gotAudioEvent);
            ofAddListener(ofEvents().update,inst,&AudioPlayer::update);
        }return inst;
    };
    
    static std::map<audioUID,ofFmodSoundPlayer*> players;
    static std::map<audioUID,float> playNeedles;
    static bool Play(Container & c,float s);
    static void Load(Container const & c,bool t);
    static void UnloadAll();
//    static bool Play(int uid,string path,float begin,float end ,ofParameter<float> & s);
    void update(ofEventArgs &a);
    void gotAudioEvent(std::pair<FMOD_CHANNEL*,FMOD_CHANNEL_CALLBACKTYPE> & ev);
    static AudioPlayer * inst;
    
    static audioUID getUID(Container const & c);
    static audioUID audioUIDfromString(const string s);
    static void stopAll();
    static float globalVolume ;
//    static ofEvent<std::pair<FMOD_CHANNEL*,FMOD_CHANNEL_CALLBACKTYPE> > stopEvent;
    
    
//    class MyPlayer : public ofFmodSoundPlayer{
//        
//        public :
//        
//        playMe(){
//            
//        }
//        
//    };
    static string errCheck(FMOD_RESULT errcode)
    {
        switch (errcode)
        {
            case FMOD_ERR_ALREADYLOCKED:          return "Tried to call lock a second time before unlock was called_ ";
            case FMOD_ERR_BADCOMMAND:             return "Tried to call a function on a data type that does not allow this type of functionality (ie calling Sound::lock on a streaming sound)_ ";
            case FMOD_ERR_CDDA_DRIVERS:           return "Neither NTSCSI nor ASPI could be initialised_ ";
            case FMOD_ERR_CDDA_INIT:              return "An error occurred while initialising the CDDA subsystem_ ";
            case FMOD_ERR_CDDA_INVALID_DEVICE:    return "Couldn't find the specified device_ ";
            case FMOD_ERR_CDDA_NOAUDIO:           return "No audio tracks on the specified disc_ ";
            case FMOD_ERR_CDDA_NODEVICES:         return "No CD/DVD devices were found_ ";
            case FMOD_ERR_CDDA_NODISC:            return "No disc present in the specified drive_ ";
            case FMOD_ERR_CDDA_READ:              return "A CDDA read error occurred_ ";
            case FMOD_ERR_CHANNEL_ALLOC:          return "Error trying to allocate a channel_ ";
            case FMOD_ERR_CHANNEL_STOLEN:         return "The specified channel has been reused to play another sound_ ";
            case FMOD_ERR_COM:                    return "A Win32 COM related error occured_ COM failed to initialize or a QueryInterface failed meaning a Windows codec or driver was not installed properly_ ";
            case FMOD_ERR_DMA:                    return "DMA Failure_  See debug output for more information_ ";
            case FMOD_ERR_DSP_CONNECTION:         return "DSP connection error_  Connection possibly caused a cyclic dependancy_ ";
            case FMOD_ERR_DSP_FORMAT:             return "DSP Format error_  A DSP unit may have attempted to connect to this network with the wrong format_ ";
            case FMOD_ERR_DSP_NOTFOUND:           return "DSP connection error_  Couldn't find the DSP unit specified_ ";
            case FMOD_ERR_DSP_RUNNING:            return "DSP error_  Cannot perform this operation while the network is in the middle of running_  This will most likely happen if a connection or disconnection is attempted in a DSP callback_ ";
            case FMOD_ERR_DSP_TOOMANYCONNECTIONS: return "DSP connection error_  The unit being connected to or disconnected should only have 1 input or output_ ";
            case FMOD_ERR_FILE_BAD:               return "Error loading file_ ";
            case FMOD_ERR_FILE_COULDNOTSEEK:      return "Couldn't perform seek operation_  This is a limitation of the medium (ie netstreams) or the file format_ ";
            case FMOD_ERR_FILE_EOF:               return "End of file unexpectedly reached while trying to read essential data (truncated data?)_ ";
            case FMOD_ERR_FILE_NOTFOUND:          return "File not found_ ";
            case FMOD_ERR_FILE_UNWANTED:          return "Unwanted file access occured_";
            case FMOD_ERR_FORMAT:                 return "Unsupported file or audio format_ ";
            case FMOD_ERR_HTTP:                   return "A HTTP error occurred_ This is a catch-all for HTTP errors not listed elsewhere_ ";
            case FMOD_ERR_HTTP_ACCESS:            return "The specified resource requires authentication or is forbidden_ ";
            case FMOD_ERR_HTTP_PROXY_AUTH:        return "Proxy authentication is required to access the specified resource_ ";
            case FMOD_ERR_HTTP_SERVER_ERROR:      return "A HTTP server error occurred_ ";
            case FMOD_ERR_HTTP_TIMEOUT:           return "The HTTP request timed out_ ";
            case FMOD_ERR_INITIALIZATION:         return "FMOD was not initialized correctly to support this function_ ";
            case FMOD_ERR_INITIALIZED:            return "Cannot call this command after System::init_ ";
            case FMOD_ERR_INTERNAL:               return "An error occured that wasnt supposed to_  Contact support_ ";
            case FMOD_ERR_INVALID_HANDLE:         return "An invalid object handle was used_ ";
            case FMOD_ERR_INVALID_PARAM:          return "An invalid parameter was passed to this function_ ";
            case FMOD_ERR_INVALID_SPEAKER:        return "An invalid speaker was passed to this function based on the current speaker mode_ ";
//            case FMOD_ERR_IRX:                    return "PS2 only_  fmodex_irx failed to initialize_  This is most likely because you forgot to load it_ ";
            case FMOD_ERR_MEMORY:                 return "Not enough memory or resources_ ";
//            case FMOD_ERR_MEMORY_IOP:             return "PS2 only_  Not enough memory or resources on PlayStation 2 IOP ram_ ";
            case FMOD_ERR_MEMORY_SRAM:            return "Not enough memory or resources on console sound ram_ ";
            case FMOD_ERR_NEEDS2D:                return "Tried to call a command on a 3d sound when the command was meant for 2d sound_ ";
            case FMOD_ERR_NEEDS3D:                return "Tried to call a command on a 2d sound when the command was meant for 3d sound_ ";
            case FMOD_ERR_NEEDSHARDWARE:          return "Tried to use a feature that requires hardware support_  (ie trying to play a VAG compressed sound in software on PS2)_ ";
            case FMOD_ERR_NEEDSSOFTWARE:          return "Tried to use a feature that requires the software engine_  Software engine has either been turned off, or command was executed on a hardware channel which does not support this feature_ ";
            case FMOD_ERR_NET_CONNECT:            return "Couldn't connect to the specified host_ ";
            case FMOD_ERR_NET_SOCKET_ERROR:       return "A socket error occurred_  This is a catch-all for socket-related errors not listed elsewhere_ ";
            case FMOD_ERR_NET_URL:                return "The specified URL couldn't be resolved_ ";
            case FMOD_ERR_NOTREADY:               return "Operation could not be performed because specified sound is not ready_ ";
            case FMOD_ERR_OUTPUT_ALLOCATED:       return "Error initializing output device, but more specifically, the output device is already in use and cannot be reused_ ";
            case FMOD_ERR_OUTPUT_CREATEBUFFER:    return "Error creating hardware sound buffer_ ";
            case FMOD_ERR_OUTPUT_DRIVERCALL:      return "A call to a standard soundcard driver failed, which could possibly mean a bug in the driver or resources were missing or exhausted_ ";
            case FMOD_ERR_OUTPUT_FORMAT:          return "Soundcard does not support the minimum features needed for this soundsystem (16bit stereo output)_ ";
            case FMOD_ERR_OUTPUT_INIT:            return "Error initializing output device_ ";
            case FMOD_ERR_OUTPUT_NOHARDWARE:      return "FMOD_HARDWARE was specified but the sound card does not have the resources nescessary to play it_ ";
            case FMOD_ERR_OUTPUT_NOSOFTWARE:      return "Attempted to create a software sound but no software channels were specified in System::init_ ";
            case FMOD_ERR_PAN:                    return "Panning only works with mono or stereo sound sources_ ";
            case FMOD_ERR_PLUGIN:                 return "An unspecified error has been returned from a 3rd party plugin_ ";
            case FMOD_ERR_PLUGIN_MISSING:         return "A requested output, dsp unit type or codec was not available_ ";
            case FMOD_ERR_PLUGIN_RESOURCE:        return "A resource that the plugin requires cannot be found_ (ie the DLS file for MIDI playback) ";
            case FMOD_ERR_RECORD:                 return "An error occured trying to initialize the recording device_ ";
            case FMOD_ERR_REVERB_INSTANCE:        return "Specified Instance in FMOD_REVERB_PROPERTIES couldn't be set_ Most likely because another application has locked the EAX4 FX slot_ ";
            case FMOD_ERR_SUBSOUNDS:              return " The error occured because the sound referenced contains subsounds_  (ie you cannot play the parent sound as a static sample, only its subsounds_)";
            case FMOD_ERR_SUBSOUND_ALLOCATED:     return "This subsound is already being used by another sound, you cannot have more than one parent to a sound_  Null out the other parent's entry first_ ";
            case FMOD_ERR_TAGNOTFOUND:            return "The specified tag could not be found or there are no tags_ ";
            case FMOD_ERR_TOOMANYCHANNELS:        return "The sound created exceeds the allowable input channel count_  This can be increased with System::setMaxInputChannels_ ";
            case FMOD_ERR_UNIMPLEMENTED:          return "Something in FMOD hasn't been implemented when it should be! contact support! ";
            case FMOD_ERR_UNINITIALIZED:          return "This command failed because System::init or System::setDriver was not called_ ";
            case FMOD_ERR_UNSUPPORTED:            return "A command issued was not supported by this object_  Possibly a plugin without certain callbacks specified_ ";
            case FMOD_ERR_UPDATE:                 return "On PS2, System::update was called twice in a row when System::updateFinished must be called first_ ";
            case FMOD_ERR_VERSION:                return "The version number of this file format is not supported_ ";
            case FMOD_OK:                         return "No errors_";
            default :                             return "Unknown error.";
        };
    }
};




#endif /* defined(__ViZa__AudioPlayer__) */
