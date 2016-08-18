//
//  OpenALStreamer.cpp
//  ViZa
//
//  Created by Martin Hermant on 18/08/2016.
//
//

#include "OpenALStreamer.hpp"


#ifdef OF_SOUND_PLAYER_OPENAL

#include "ofUtils.h"
#include "ofMath.h"
#include "ofFileUtils.h"
#include "ofAppRunner.h"
#include <set>

ALCdevice * OpenALStreamer::alDevice = 0;
ALCcontext * OpenALStreamer::alContext = 0;
vector<float> OpenALStreamer::window;
float OpenALStreamer::windowSum=0;


//kiss_fftr_cfg OpenALStreamer::systemFftCfg=0;
//vector<float> OpenALStreamer::systemWindowedSignal;
//vector<float> OpenALStreamer::systemBins;
//vector<kiss_fft_cpx> OpenALStreamer::systemCx_out;

static set<OpenALStreamer*> & players(){
  static set<OpenALStreamer*> * players = new set<OpenALStreamer*>;
  return *players;
}

void ofOpenALSoundUpdate(){
  alcProcessContext(OpenALStreamer::alContext);
}

// ----------------------------------------------------------------------------
// from http://devmaster.net/posts/2893/openal-lesson-6-advanced-loading-and-error-handles
static string getALErrorString(ALenum error) {
  switch(error) {
    case AL_NO_ERROR:
      return "AL_NO_ERROR";
    case AL_INVALID_NAME:
      return "AL_INVALID_NAME";
    case AL_INVALID_ENUM:
      return "AL_INVALID_ENUM";
    case AL_INVALID_VALUE:
      return "AL_INVALID_VALUE";
    case AL_INVALID_OPERATION:
      return "AL_INVALID_OPERATION";
    case AL_OUT_OF_MEMORY:
      return "AL_OUT_OF_MEMORY";
  };
  return "UNKWOWN_ERROR";
}

#ifdef OF_USING_MPG123
static string getMpg123EncodingString(int encoding) {
  switch(encoding) {
    case MPG123_ENC_16:
      return "MPG123_ENC_16";
#if MPG123_API_VERSION>=36
    case MPG123_ENC_24:
      return "MPG123_ENC_24";
#endif
    case MPG123_ENC_32:
      return "MPG123_ENC_32";
    case MPG123_ENC_8:
      return "MPG123_ENC_8";
    case MPG123_ENC_ALAW_8:
      return "MPG123_ENC_ALAW_8";
    case MPG123_ENC_FLOAT:
      return "MPG123_ENC_FLOAT";
    case MPG123_ENC_FLOAT_32:
      return "MPG123_ENC_FLOAT_32";
    case MPG123_ENC_FLOAT_64:
      return "MPG123_ENC_FLOAT_64";
    case MPG123_ENC_SIGNED:
      return "MPG123_ENC_SIGNED";
    case MPG123_ENC_SIGNED_16:
      return "MPG123_ENC_SIGNED_16";
#if MPG123_API_VERSION>=36
    case MPG123_ENC_SIGNED_24:
      return "MPG123_ENC_SIGNED_24";
#endif
    case MPG123_ENC_SIGNED_32:
      return "MPG123_ENC_SIGNED_32";
    case MPG123_ENC_SIGNED_8:
      return "MPG123_ENC_SIGNED_8";
    case MPG123_ENC_ULAW_8:
      return "MPG123_ENC_ULAW_8";
    case MPG123_ENC_UNSIGNED_16:
      return "MPG123_ENC_UNSIGNED_16";
#if MPG123_API_VERSION>=36
    case MPG123_ENC_UNSIGNED_24:
      return "MPG123_ENC_UNSIGNED_24";
#endif
    case MPG123_ENC_UNSIGNED_32:
      return "MPG123_ENC_UNSIGNED_32";
    case MPG123_ENC_UNSIGNED_8:
      return "MPG123_ENC_UNSIGNED_8";
    default:
      return "MPG123_ENC_ANY";
  }
}
#endif

#define BUFFER_STREAM_SIZE 128

// now, the individual sound player:
//------------------------------------------------------------
OpenALStreamer::OpenALStreamer(){
  bLoop 			= false;
  bLoadedOk 		= false;
  pan 			= 0.0f; // range for oF is -1 to 1,
  volume 			= 1.0f;
  internalFreq 	= 44100;
  speed 			= 1;
  bPaused 		= false;
  isStreaming		= false;
  channels		= 0;
  duration		= 0;
  streamf			= 0;
#ifdef OF_USING_MPG123
  mp3streamf		= 0;
#endif
  players().insert(this);
}

// ----------------------------------------------------------------------------
OpenALStreamer::~OpenALStreamer(){
  unload();

  players().erase(this);
}

//---------------------------------------
// this should only be called once
void OpenALStreamer::initialize(){
  if(!alDevice){
    alDevice = alcOpenDevice(nullptr);
    alContext = alcCreateContext(alDevice,nullptr);
    alcMakeContextCurrent (alContext);
    alListener3f(AL_POSITION, 0,0,0);
#ifdef OF_USING_MPG123
    mpg123_init();
#endif

  }
}



//---------------------------------------
void OpenALStreamer::close(){
  if(alDevice){
    alcCloseDevice(alDevice);
    alDevice = nullptr;
    alcDestroyContext(alContext);
    alContext = 0;
#ifdef OF_USING_MPG123
    mpg123_exit();
#endif
  }
}

// ----------------------------------------------------------------------------
bool OpenALStreamer::sfReadFile(string path, vector<short> & buffer, vector<float> & fftAuxBuffer){
  SF_INFO sfInfo;
  SNDFILE* f = sf_open(path.c_str(),SFM_READ,&sfInfo);
  if(!f){
    ofLogError("OpenALStreamer") << "sfReadFile(): couldn't read \"" << path << "\"";
    return false;
  }

  buffer.resize(sfInfo.frames*sfInfo.channels);
  fftAuxBuffer.resize(sfInfo.frames*sfInfo.channels);

  int subformat = sfInfo.format & SF_FORMAT_SUBMASK ;
  if (subformat == SF_FORMAT_FLOAT || subformat == SF_FORMAT_DOUBLE){
    double	scale ;
    sf_command (f, SFC_CALC_SIGNAL_MAX, &scale, sizeof (scale)) ;
    if (scale < 1e-10)
      scale = 1.0 ;
    else
      scale = 32700.0 / scale ;

    sf_count_t samples_read = sf_read_float (f, &fftAuxBuffer[0], fftAuxBuffer.size());
    if(samples_read<(int)fftAuxBuffer.size()){
      ofLogWarning("OpenALStreamer") << "sfReadFile(): read " << samples_read << " float samples, expected "
      << fftAuxBuffer.size() << " for \"" << path << "\"";
    }
    for (int i = 0 ; i < int(fftAuxBuffer.size()) ; i++){
      fftAuxBuffer[i] *= scale ;
      buffer[i] = 32565.0 * fftAuxBuffer[i];
    }
  }else{
    sf_count_t frames_read = sf_readf_short(f,&buffer[0],sfInfo.frames);
    if(frames_read<sfInfo.frames){
      ofLogError("OpenALStreamer") << "sfReadFile(): read " << frames_read << " frames from buffer, expected "
      << sfInfo.frames << " for \"" << path << "\"";
      return false;
    }
    sf_seek(f,0,SEEK_SET);
    frames_read = sf_readf_float(f,&fftAuxBuffer[0],sfInfo.frames);
    if(frames_read<sfInfo.frames){
      ofLogError("OpenALStreamer") << "sfReadFile(): read " << frames_read << " frames from fft buffer, expected "
      << sfInfo.frames << " for \"" << path << "\"";
      return false;
    }
  }
  sf_close(f);

  channels = sfInfo.channels;
  duration = float(sfInfo.frames) / float(sfInfo.samplerate);
  samplerate = sfInfo.samplerate;
  return true;
}

#ifdef OF_USING_MPG123
//------------------------------------------------------------
bool OpenALStreamer::mpg123ReadFile(string path,vector<short> & buffer,vector<float> & fftAuxBuffer){
  int err = MPG123_OK;
  mpg123_handle * f = mpg123_new(nullptr,&err);
  if(mpg123_open(f,path.c_str())!=MPG123_OK){
    ofLogError("OpenALStreamer") << "mpg123ReadFile(): couldn't read \"" << path << "\"";
    return false;
  }

  mpg123_enc_enum encoding;
  long int rate;
  mpg123_getformat(f,&rate,&channels,(int*)&encoding);
  if(encoding!=MPG123_ENC_SIGNED_16){
    ofLogError("OpenALStreamer") << "mpg123ReadFile(): " << getMpg123EncodingString(encoding)
    << " encoding for \"" << path << "\"" << " unsupported, expecting MPG123_ENC_SIGNED_16";
    return false;
  }
  samplerate = rate;

  size_t done=0;
  size_t buffer_size = mpg123_outblock( f );
  buffer.resize(buffer_size/2);
  while(mpg123_read(f,(unsigned char*)&buffer[buffer.size()-buffer_size/2],buffer_size,&done)!=MPG123_DONE){
    buffer.resize(buffer.size()+buffer_size/2);
  };
  buffer.resize(buffer.size()-(buffer_size/2-done/2));
  mpg123_close(f);
  mpg123_delete(f);

  fftAuxBuffer.resize(buffer.size());
  for(int i=0;i<(int)buffer.size();i++){
    fftAuxBuffer[i] = float(buffer[i])/32565.f;
  }
  duration = float(buffer.size()/channels) / float(samplerate);
  return true;
}
#endif

//------------------------------------------------------------
bool OpenALStreamer::sfStream(string path,vector<short> & buffer,vector<float> & fftAuxBuffer){
  if(!streamf){
    SF_INFO sfInfo;
    streamf = sf_open(path.c_str(),SFM_READ,&sfInfo);
    if(!streamf){
      ofLogError("OpenALStreamer") << "sfStream(): couldn't read \"" << path << "\"";
      return false;
    }

    stream_subformat = sfInfo.format & SF_FORMAT_SUBMASK ;
    if (stream_subformat == SF_FORMAT_FLOAT || stream_subformat == SF_FORMAT_DOUBLE){
      sf_command (streamf, SFC_CALC_SIGNAL_MAX, &stream_scale, sizeof (stream_scale)) ;
      if (stream_scale < 1e-10)
        stream_scale = 1.0 ;
      else
        stream_scale = 32700.0 / stream_scale ;
    }
    channels = sfInfo.channels;
    duration = float(sfInfo.frames) / float(sfInfo.samplerate);
    samplerate = sfInfo.samplerate;
    stream_samples_read = 0;
  }

  int curr_buffer_size = BUFFER_STREAM_SIZE*channels;
  if(speed>1) curr_buffer_size *= (int)round(speed);
  buffer.resize(curr_buffer_size);
  fftAuxBuffer.resize(buffer.size());
  if (stream_subformat == SF_FORMAT_FLOAT || stream_subformat == SF_FORMAT_DOUBLE){
    sf_count_t samples_read = sf_read_float (streamf, &fftAuxBuffer[0], fftAuxBuffer.size());
    stream_samples_read += samples_read;
    if(samples_read<(int)fftAuxBuffer.size()){
      fftAuxBuffer.resize(samples_read);
      buffer.resize(samples_read);
      setPosition(0);
      if(!bLoop) stopThread();
      stream_samples_read = 0;
      stream_end = true;
    }
    for (int i = 0 ; i < int(fftAuxBuffer.size()) ; i++){
      fftAuxBuffer[i] *= stream_scale ;
      buffer[i] = 32565.0 * fftAuxBuffer[i];
    }
  }else{
    sf_count_t frames_read = sf_readf_short(streamf,&buffer[0],curr_buffer_size/channels);
    stream_samples_read += frames_read*channels;
    if(frames_read<curr_buffer_size/channels){
      fftAuxBuffer.resize(frames_read*channels);
      buffer.resize(frames_read*channels);
      setPosition(0);
      if(!bLoop) stopThread();
      stream_samples_read = 0;
      stream_end = true;
    }
    for(int i=0;i<(int)buffer.size();i++){
      fftAuxBuffer[i]=float(buffer[i])/32565.0f;
    }
  }

  return true;
}

#ifdef OF_USING_MPG123
//------------------------------------------------------------
bool OpenALStreamer::mpg123Stream(string path,vector<short> & buffer,vector<float> & fftAuxBuffer){
  if(!mp3streamf){
    int err = MPG123_OK;
    mp3streamf = mpg123_new(nullptr,&err);
    if(mpg123_open(mp3streamf,path.c_str())!=MPG123_OK){
      mpg123_close(mp3streamf);
      mpg123_delete(mp3streamf);
      ofLogError("OpenALStreamer") << "mpg123Stream(): couldn't read \"" << path << "\"";
      return false;
    }

    long int rate;
    mpg123_getformat(mp3streamf,&rate,&channels,(int*)&stream_encoding);
    if(stream_encoding!=MPG123_ENC_SIGNED_16){
      ofLogError("OpenALStreamer") << "mpg123Stream(): " << getMpg123EncodingString(stream_encoding)
      << " encoding for \"" << path << "\"" << " unsupported, expecting MPG123_ENC_SIGNED_16";
      return false;
    }
    samplerate = rate;
    mp3_buffer_size = mpg123_outblock( mp3streamf );


    mpg123_seek(mp3streamf,0,SEEK_END);
    off_t samples = mpg123_tell(mp3streamf);
    duration = float(samples/channels) / float(samplerate);
    mpg123_seek(mp3streamf,0,SEEK_SET);
  }

  int curr_buffer_size = mp3_buffer_size;
  if(speed>1) curr_buffer_size *= (int)round(speed);
  buffer.resize(curr_buffer_size);
  fftAuxBuffer.resize(buffer.size());
  size_t done=0;
  if(mpg123_read(mp3streamf,(unsigned char*)&buffer[0],curr_buffer_size*2,&done)==MPG123_DONE){
    setPosition(0);
    buffer.resize(done/2);
    fftAuxBuffer.resize(done/2);
    if(!bLoop) stopThread();
    stream_end = true;
  }


  for(int i=0;i<(int)buffer.size();i++){
    fftAuxBuffer[i] = float(buffer[i])/32565.f;
  }

  return true;
}
#endif

//------------------------------------------------------------
bool OpenALStreamer::stream(string fileName, vector<short> & buffer){
#ifdef OF_USING_MPG123
  if(ofFilePath::getFileExt(fileName)=="mp3" || ofFilePath::getFileExt(fileName)=="MP3" || mp3streamf){
    if(!mpg123Stream(fileName,buffer,fftAuxBuffer)) return false;
  }else
#endif
    if(!sfStream(fileName,buffer,fftAuxBuffer)) return false;

  fftBuffers.resize(channels);
  int numFrames = buffer.size()/channels;

  for(int i=0;i<channels;i++){
    fftBuffers[i].resize(numFrames);
    for(int j=0;j<numFrames;j++){
      fftBuffers[i][j] = fftAuxBuffer[j*channels+i];
    }
  }
  return true;
}

bool OpenALStreamer::readFile(string fileName, vector<short> & buffer){
#ifdef OF_USING_MPG123
  if(ofFilePath::getFileExt(fileName)!="mp3" && ofFilePath::getFileExt(fileName)!="MP3"){
    if(!sfReadFile(fileName,buffer,fftAuxBuffer)) return false;
  }else{
    if(!mpg123ReadFile(fileName,buffer,fftAuxBuffer)) return false;
  }
#else
  if(!sfReadFile(fileName,buffer,fftAuxBuffer)) return false;
#endif
  fftBuffers.resize(channels);
  int numFrames = buffer.size()/channels;

  for(int i=0;i<channels;i++){
    fftBuffers[i].resize(numFrames);
    for(int j=0;j<numFrames;j++){
      fftBuffers[i][j] = fftAuxBuffer[j*channels+i];
    }
  }
  return true;
}

//------------------------------------------------------------
bool OpenALStreamer::load(string fileName, bool is_stream){

  fileName = ofToDataPath(fileName);

  bMultiPlay = false;
  isStreaming = is_stream;
  int err = AL_NO_ERROR;

  // [1] init sound systems, if necessary
  initialize();

  // [2] try to unload any previously loaded sounds
  // & prevent user-created memory leaks
  // if they call "loadSound" repeatedly, for example

  unload();
  ALenum format=AL_FORMAT_MONO16;
  bLoadedOk = false;

  if(!isStreaming){
    readFile(fileName, buffer);
  }else{
    stream(fileName, buffer);
  }

  int numFrames = buffer.size()/channels;

  if(isStreaming){
    buffers.resize(channels*2);
  }else{
    buffers.resize(channels);
  }
  alGenBuffers(buffers.size(), &buffers[0]);
  if(channels==1){
    sources.resize(1);
    alGetError(); // Clear error.
    alGenSources(1, &sources[0]);
    err = alGetError();
    if (err != AL_NO_ERROR){
      ofLogError("OpenALStreamer") << "loadSound(): couldn't generate source for \"" << fileName << "\": "
      << (int) err << " " << getALErrorString(err);
      return false;
    }

    for(int i=0; i<(int)buffers.size(); i++){
      alGetError(); // Clear error.
      alBufferData(buffers[i],format,&buffer[0],buffer.size()*2,samplerate);
      err = alGetError();
      if (err != AL_NO_ERROR){
        ofLogError("OpenALStreamer:") << "loadSound(): couldn't create buffer for \"" << fileName << "\": "
        << (int) err << " " << getALErrorString(err);
        return false;
      }
      if(isStreaming){
        stream(fileName,buffer);
      }
    }
    if(isStreaming){
      alSourceQueueBuffers(sources[0],buffers.size(),&buffers[0]);
    }else{
      alSourcei (sources[0], AL_BUFFER,   buffers[0]);
    }

    alSourcef (sources[0], AL_PITCH,    1.0f);
    alSourcef (sources[0], AL_GAIN,     1.0f);
    alSourcef (sources[0], AL_ROLLOFF_FACTOR,  0.0);
    alSourcei (sources[0], AL_SOURCE_RELATIVE, AL_TRUE);
  }else{
    vector<vector<short> > multibuffer;
    multibuffer.resize(channels);
    sources.resize(channels);
    alGenSources(channels, &sources[0]);
    if(isStreaming){
      for(int s=0; s<2;s++){
        for(int i=0;i<channels;i++){
          multibuffer[i].resize(buffer.size()/channels);
          for(int j=0;j<numFrames;j++){
            multibuffer[i][j] = buffer[j*channels+i];
          }
          alGetError(); // Clear error.
          alBufferData(buffers[s*2+i],format,&multibuffer[i][0],buffer.size()/channels*2,samplerate);
          err = alGetError();
          if ( err != AL_NO_ERROR){
            ofLogError("OpenALStreamer") << "loadSound(): couldn't create stereo buffers for \"" << fileName << "\": " << (int) err << " " << getALErrorString(err);
            return false;
          }
          alSourceQueueBuffers(sources[i],1,&buffers[s*2+i]);
          stream(fileName,buffer);
        }
      }
    }else{
      for(int i=0;i<channels;i++){
        multibuffer[i].resize(buffer.size()/channels);
        for(int j=0;j<numFrames;j++){
          multibuffer[i][j] = buffer[j*channels+i];
        }
        alGetError(); // Clear error.
        alBufferData(buffers[i],format,&multibuffer[i][0],buffer.size()/channels*2,samplerate);
        err = alGetError();
        if (err != AL_NO_ERROR){
          ofLogError("OpenALStreamer") << "loadSound(): couldn't create stereo buffers for \"" << fileName << "\": "
          << (int) err << " " << getALErrorString(err);
          return false;
        }
        alSourcei (sources[i], AL_BUFFER,   buffers[i]   );
      }
    }


    for(int i=0;i<channels;i++){
      err = alGetError();
      if (err != AL_NO_ERROR){
        ofLogError("OpenALStreamer") << "loadSound(): couldn't create stereo sources for \"" << fileName << "\": "
        << (int) err << " " << getALErrorString(err);
        return false;
      }

      // only stereo panning
      if(i==0){
        float pos[3] = {-1,0,0};
        alSourcefv(sources[i],AL_POSITION,pos);
      }else{
        float pos[3] = {1,0,0};
        alSourcefv(sources[i],AL_POSITION,pos);
      }
      alSourcef (sources[i], AL_ROLLOFF_FACTOR,  0.0);
      alSourcei (sources[i], AL_SOURCE_RELATIVE, AL_TRUE);
    }
  }

  bLoadedOk = true;
  return bLoadedOk;

}

//------------------------------------------------------------
bool OpenALStreamer::isLoaded() const{
  return bLoadedOk;
}

//------------------------------------------------------------
void OpenALStreamer::threadedFunction(){
  vector<vector<short> > multibuffer;
  multibuffer.resize(channels);
  while(isThreadRunning()){
    std::unique_lock<std::mutex> lock(mutex);
    for(int i=0; i<int(sources.size())/channels; i++){
      int processed;
      alGetSourcei(sources[i*channels], AL_BUFFERS_PROCESSED, &processed);

      while(processed--)
      {
        stream("",buffer);
        int numFrames = buffer.size()/channels;
        if(channels>1){
          for(int j=0;j<channels;j++){
            multibuffer[j].resize(buffer.size()/channels);
            for(int k=0;k<numFrames;k++){
              multibuffer[j][k] = buffer[k*channels+j];
            }
            ALuint albuffer;
            alSourceUnqueueBuffers(sources[i*channels+j], 1, &albuffer);
            alBufferData(albuffer,AL_FORMAT_MONO16,&multibuffer[j][0],buffer.size()*2/channels,samplerate);
            alSourceQueueBuffers(sources[i*channels+j], 1, &albuffer);
          }
        }else{
          ALuint albuffer;
          alSourceUnqueueBuffers(sources[i], 1, &albuffer);
          alBufferData(albuffer,AL_FORMAT_MONO16,&buffer[0],buffer.size()*2/channels,samplerate);
          alSourceQueueBuffers(sources[i], 1, &albuffer);
        }
        if(stream_end){
          break;
        }
      }
      ALint state;
      alGetSourcei(sources[i*channels],AL_SOURCE_STATE,&state);
      bool stream_running=false;
#ifdef OF_USING_MPG123
      stream_running = streamf || mp3streamf;
#else
      stream_running = streamf;
#endif
      if(state != AL_PLAYING && stream_running && !stream_end){
        alSourcePlayv(channels,&sources[i*channels]);
      }

      stream_end = false;
    }
    sleep(1);
  }
}

//------------------------------------------------------------
void OpenALStreamer::update(ofEventArgs & args){

  for(int i=1; i<int(sources.size())/channels; ){
    ALint state;
    alGetSourcei(sources[i*channels],AL_SOURCE_STATE,&state);
    if(state != AL_PLAYING){
      alDeleteSources(channels,&sources[i*channels]);
      for(int j=0;j<channels;j++){
        sources.erase(sources.begin()+i*channels);
      }
    }else{
      i++;
    }
  }
}

//------------------------------------------------------------
void OpenALStreamer::unload(){
  stop();
  ofRemoveListener(ofEvents().update,this,&OpenALStreamer::update);

  // Only lock the thread where necessary.
  {
    std::unique_lock<std::mutex> lock(mutex);

    // Delete sources before buffers.
    alDeleteSources(sources.size(),&sources[0]);
    alDeleteBuffers(buffers.size(),&buffers[0]);

    sources.clear();
    buffers.clear();
  }

  // Free resources and close file descriptors.
#ifdef OF_USING_MPG123
  if(mp3streamf){
    mpg123_close(mp3streamf);
    mpg123_delete(mp3streamf);
  }
  mp3streamf = 0;
#endif

  if(streamf){
    sf_close(streamf);
  }
  streamf = 0;

  bLoadedOk = false;
}

//------------------------------------------------------------
bool OpenALStreamer::isPlaying() const{
  if(sources.empty()) return false;
  if(isStreaming) return isThreadRunning();
  ALint state;
  bool playing=false;
  for(int i=0;i<(int)sources.size();i++){
    alGetSourcei(sources[i],AL_SOURCE_STATE,&state);
    playing |= (state == AL_PLAYING);
  }
  return playing;
}

//------------------------------------------------------------
bool OpenALStreamer::isPaused() const{
  if(sources.empty()) return false;
  ALint state;
  bool paused=true;
  for(int i=0;i<(int)sources.size();i++){
    alGetSourcei(sources[i],AL_SOURCE_STATE,&state);
    paused &= (state == AL_PAUSED);
  }
  return paused;
}

//------------------------------------------------------------
float OpenALStreamer::getSpeed() const{
  return speed;
}

//------------------------------------------------------------
float OpenALStreamer::getPan() const{
  return pan;
}

//------------------------------------------------------------
float OpenALStreamer::getVolume() const{
  return volume;
}

//------------------------------------------------------------
void OpenALStreamer::setVolume(float vol){
  volume = vol;
  if(sources.empty()) return;
  if(channels==1){
    alSourcef (sources[sources.size()-1], AL_GAIN, vol);
  }else{
    setPan(pan);
  }
}

//------------------------------------------------------------
void OpenALStreamer::setPosition(float pct){
  setPositionMS(duration*pct*1000.f);
}

//------------------------------------------------------------
void OpenALStreamer::setPositionMS(int ms){
  if(sources.empty()) return;
#ifdef OF_USING_MPG123
  if(mp3streamf){
    mpg123_seek(mp3streamf,float(ms)/1000.f*samplerate,SEEK_SET);
  }else
#endif
    if(streamf){
      sf_seek(streamf,float(ms)/1000.f*samplerate*channels,SEEK_SET);
      stream_samples_read = 0;
    }else{
      for(int i=0;i<(int)channels;i++){
        alSourcef(sources[sources.size()-channels+i],AL_SEC_OFFSET,float(ms)/1000.f);
      }
    }
}

//------------------------------------------------------------
float OpenALStreamer::getPosition() const{
  if(duration==0 || sources.empty())
    return 0;
  else
    return getPositionMS()/(1000.f*duration);
}

//------------------------------------------------------------
int OpenALStreamer::getPositionMS() const{
  if(sources.empty()) return 0;
  float pos;
#ifdef OF_USING_MPG123
  if(mp3streamf){
    pos = float(mpg123_tell(mp3streamf)) / float(samplerate);
  }else
#endif
    if(streamf){
      pos = float(stream_samples_read) / float(channels) / float(samplerate);
    }else{
      alGetSourcef(sources[sources.size()-1],AL_SEC_OFFSET,&pos);
    }
  return pos * 1000.f;
}

//------------------------------------------------------------
void OpenALStreamer::setPan(float p){
  if(sources.empty()) return;
  p = ofClamp(p, -1, 1);
  pan = p;
  if(channels==1){
    float pos[3] = {p,0,0};
    alSourcefv(sources[sources.size()-1],AL_POSITION,pos);
  }else{
    // calculates left/right volumes from pan-value (constant panning law)
    // see: Curtis Roads: Computer Music Tutorial p 460
    // thanks to jasch
    float angle = p * 0.7853981633974483f; // in radians from -45. to +45.
    float cosAngle = cos(angle);
    float sinAngle = sin(angle);
    float leftVol  = (cosAngle - sinAngle) * 0.7071067811865475; // multiplied by sqrt(2)/2
    float rightVol = (cosAngle + sinAngle) * 0.7071067811865475; // multiplied by sqrt(2)/2
    for(int i=0;i<(int)channels;i++){
      if(i==0){
        alSourcef(sources[sources.size()-channels+i],AL_GAIN,leftVol*volume);
      }else{
        alSourcef(sources[sources.size()-channels+i],AL_GAIN,rightVol*volume);
      }
    }
  }
}


//------------------------------------------------------------
void OpenALStreamer::setPaused(bool bP){
  if(sources.empty()) return;
  std::unique_lock<std::mutex> lock(mutex);
  if(bP){
    alSourcePausev(sources.size(),&sources[0]);
    if(isStreaming){
      stopThread();
    }
  }else{
    alSourcePlayv(sources.size(),&sources[0]);
    if(isStreaming){
      startThread();
    }
  }

  bPaused = bP;
}


//------------------------------------------------------------
void OpenALStreamer::setSpeed(float spd){
  for(int i=0;i<channels;i++){
    alSourcef(sources[sources.size()-channels+i],AL_PITCH,spd);
  }
  speed = spd;
}


//------------------------------------------------------------
void OpenALStreamer::setLoop(bool bLp){
  if(bMultiPlay) return; // no looping on multiplay
  bLoop = bLp;
  if(isStreaming) return;
  for(int i=0;i<(int)sources.size();i++){
    alSourcei(sources[i],AL_LOOPING,bLp?AL_TRUE:AL_FALSE);
  }
}

// ----------------------------------------------------------------------------
void OpenALStreamer::setMultiPlay(bool bMp){
  if(isStreaming && bMp){
    ofLogWarning("OpenALStreamer") << "setMultiPlay(): sorry, no support for multiplay streams";
    return;
  }
  bMultiPlay = bMp;		// be careful with this...
  if(sources.empty()) return;
  if(bMultiPlay){
    ofAddListener(ofEvents().update,this,&OpenALStreamer::update);
  }else{
    ofRemoveListener(ofEvents().update,this,&OpenALStreamer::update);
  }
}

// ----------------------------------------------------------------------------
void OpenALStreamer::play(){
  {
    std::unique_lock<std::mutex> lock(mutex);
    int err = glGetError();

    // if the sound is set to multiplay, then create new sources,
    // do not multiplay on loop or we won't be able to stop it
    if (bMultiPlay && !bLoop){
      sources.resize(sources.size()+channels);
      alGetError(); // Clear error.
      alGenSources(channels, &sources[sources.size()-channels]);
      err = alGetError();
      if (err != AL_NO_ERROR){
        ofLogError("OpenALStreamer") << "play(): couldn't create multiplay stereo sources: "
        << (int) err << " " << getALErrorString(err);
        return;
      }
      for(int i=0;i<channels;i++){
        alSourcei (sources[sources.size()-channels+i], AL_BUFFER,   buffers[i]   );
        // only stereo panning
        if(i==0){
          float pos[3] = {-1,0,0};
          alSourcefv(sources[sources.size()-channels+i],AL_POSITION,pos);
        }else{
          float pos[3] = {1,0,0};
          alSourcefv(sources[sources.size()-channels+i],AL_POSITION,pos);
        }
        alSourcef (sources[sources.size()-channels+i], AL_ROLLOFF_FACTOR,  0.0);
        alSourcei (sources[sources.size()-channels+i], AL_SOURCE_RELATIVE, AL_TRUE);
      }

      err = glGetError();
      if (err != AL_NO_ERROR){
        ofLogError("OpenALStreamer") << "play(): couldn't assign multiplay buffers: "
        << (int) err << " " << getALErrorString(err);
        return;
      }
    }

    alSourcePlayv(channels,&sources[sources.size()-channels]);
  }
  if(bMultiPlay){
    ofAddListener(ofEvents().update,this,&OpenALStreamer::update);
  }
  if(isStreaming){
    setPosition(0);
    stream_end = false;
    startThread(true);
  }

}

// ----------------------------------------------------------------------------
void OpenALStreamer::stop(){
  if(sources.empty()) return;
  std::unique_lock<std::mutex> lock(mutex);
  alSourceStopv(channels,&sources[sources.size()-channels]);
  if(isStreaming){
    setPosition(0);
    stopThread();
  }
}








#endif
