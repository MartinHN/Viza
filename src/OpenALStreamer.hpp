//
//  OpenALStreamer.hpp
//  ViZa
//
//  Created by Martin Hermant on 18/08/2016.
//
//

#ifndef OpenALStreamer_hpp
#define OpenALStreamer_hpp



#include "ofConstants.h"


#include "ofBaseSoundPlayer.h"
#include "ofEvents.h"
#include "ofThread.h"

#if defined (TARGET_OF_IOS) || defined (TARGET_OSX)
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif


#include <sndfile.h>
#ifdef OF_USING_MPG123
#include <mpg123.h>
#endif

//		TO DO :
//		---------------------------
// 		-fft via fmod, as in the last time...
// 		-close fmod if it's up
//		-loadSoundForStreaming(char * fileName);
//		---------------------------

// 		interesting:
//		http://www.compuphase.com/mp3/mp3loops.htm


// ---------------------------------------------------------------------------- SOUND SYSTEM FMOD

// --------------------- global functions:
void ofFmodSoundStopAll();
void ofFmodSoundSetVolume(float vol);
void ofOpenALSoundUpdate();						// calls FMOD update.



// --------------------- player functions:
class OpenALStreamer : public ofBaseSoundPlayer, public ofThread {

public:

		OpenALStreamer();
		virtual ~OpenALStreamer();


		bool load(string fileName, bool stream = false);
		void unload();
		void play();
		void stop();

		void setVolume(float vol);
		void setPan(float vol); // -1 to 1
		void setSpeed(float spd);
		void setPaused(bool bP);
		void setLoop(bool bLp);
		void setMultiPlay(bool bMp);
		void setPosition(float pct); // 0 = start, 1 = end;
		void setPositionMS(int ms);


		float getPosition() const;
		int getPositionMS() const;
		bool isPlaying() const;
		float getSpeed() const;
		float getPan() const;
    float getVolume() const;
		bool isPaused() const;
		bool isLoaded() const;

		static void initialize();
		static void close();


protected:
		void threadedFunction();

private:
		friend void ofOpenALSoundUpdate();
		void update(ofEventArgs & args);

		float * getCurrentBufferSum(int size);

		bool sfReadFile(string path,vector<short> & buffer,vector<float> & fftAuxBuffer);
		bool sfStream(string path,vector<short> & buffer,vector<float> & fftAuxBuffer);
#ifdef OF_USING_MPG123
		bool mpg123ReadFile(string path,vector<short> & buffer,vector<float> & fftAuxBuffer);
		bool mpg123Stream(string path,vector<short> & buffer,vector<float> & fftAuxBuffer);
#endif

		bool readFile(string fileName,vector<short> & buffer);
		bool stream(string fileName, vector<short> & buffer);

		bool isStreaming;
		bool bMultiPlay;
		bool bLoop;
		bool bLoadedOk;
		bool bPaused;
		float pan; // 0 - 1
		float volume; // 0 - 1
		float internalFreq; // 44100 ?
		float speed; // -n to n, 1 = normal, -1 backwards
		unsigned int length; // in samples;

		static ALCdevice * alDevice;
		static ALCcontext * alContext;


		int channels;
		float duration; //in secs
		int samplerate;
		vector<ALuint> buffers;
		vector<ALuint> sources;
  typedef struct player{
    ALuint buffer;
    ALuint source;
    string file;
  };
  vector<player> players;


		SNDFILE* streamf;
		size_t stream_samples_read;
#ifdef OF_USING_MPG123
		mpg123_handle * mp3streamf;
		int stream_encoding;
#endif
		int mp3_buffer_size;
		int stream_subformat;
		double stream_scale;
		vector<short> buffer;

  
		bool stream_end;
};



#endif /* OpenALStreamer_hpp */
