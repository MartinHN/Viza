//
//  AudioPlayer.cpp
//  ViZa
//
//  Created by martin hermant on 09/10/14.
//
//

#include "AudioPlayer.h"
#ifdef USE_FMODPLAYER
#include "ofFmodSoundPlayer.cpp"
#else
// defaults to openAL


#endif


#define DEBUG_AUDIO

#ifdef DEBUG_AUDIO
#define DEBUGPRINT_AUDIO(x) ofLogWarning("Audio") << x;
#else
#define DEBUGPRINT_AUDIO(x) ;
#endif


#define STOPNEEDLE -100000


std::map<audioUID,AudioPlayer::PLAYERPTR>  AudioPlayer::players;
std::map<audioUID,float> AudioPlayer::playNeedles;
AudioPlayer * AudioPlayer::inst;
float AudioPlayer::globalVolume = 0.6;
float AudioPlayer::maxTime = 0;
ofMutex AudioPlayer::staticMutex;

AudioPlayer::AudioPlayer(){

#ifdef USE_FMODPLAYER
  buffersize = 256;

  ofFmodSoundPlayer::initializeFmod();
#endif

}

void AudioPlayer::update(ofEventArgs & a){

  //    ofFmodSoundUpdate();
  std::lock_guard<std::mutex> sl(staticMutex);
  vector<audioUID > toRemove;
  for(auto & n:playNeedles){
    cout << players[n.first] << endl;
    if(players.count(n.first)){
      if(auto cPlayer = players[n.first]){
        
    cout  << cPlayer->getPositionMS() <<","<< n.second*1000.0  << endl;

    if(players.count(n.first)>0){
      int isPlaying = 1;
#ifdef USE_FMODPLAYER
      FMOD_Channel_IsPlaying(players[n.first]->channel,&isPlaying);
#else
      isPlaying = cPlayer->isPlaying()?1:0;
#endif

      if(isPlaying==0 ||
         (!cPlayer->isLoaded()) ||
         (cPlayer->getPositionMS() > n.second*1000.0 ) ||
         n.second==STOPNEEDLE
         ){
          ofLogWarning("Audio") << "stopping : " << n.first.toString();

          cPlayer->stop();
          Container::containers[n.first.idx]->state = 0;

          n.second = STOPNEEDLE;
          toRemove.push_back(n.first);

        }
    }


  }
    else{

      ofLogError("Audio") << " dirty pointer : " << n.first.toString();
    }
    }
  }
  for(auto &r:toRemove){
    if(players.count(r)){
      players[r]->stop();
//      players[r] = nullptr;
//      players.erase(r);
    }
  }
  //  remove garbage
  for(auto & pn:playNeedles){
    bool found = false;
    for (auto & p :players){
      found|= (pn.first == p.first);
    }
    if(!found)
      toRemove.push_back(pn.first);
  }
  for(auto &r:toRemove){
    playNeedles.erase(r);
  }
//  {
//
//    vector<map<audioUID,AudioPlayer::PLAYERPTR>::iterator> playerToRemove;
//    for (std::map<audioUID,AudioPlayer::PLAYERPTR>::iterator p = players.begin() ; p!=players.end() ;++p){
//      if(!p->second)playerToRemove.push_back(p);
//    }
//    for (auto & p:playerToRemove){
//      players.erase(p);
//    }
//  }
}
#ifdef USE_FMODPLAYER
FMOD_RESULT AudioPlayer::gotFmodEvent(FMOD_CHANNEL *channel, FMOD_CHANNEL_CALLBACKTYPE type, void *commanddata1, void *commanddata2){
  lock_guard<mutex> lk(staticMutex);
  for (auto & p:players){
    if(p.second->channel == channel && type==FMOD_CHANNEL_CALLBACKTYPE_END){
      playNeedles[p.first] = STOPNEEDLE;
      Container::containers[p.first.idx]->setState(0);
    }
  }
}
#endif

void AudioPlayer::Load(Container const & c,bool t){
  audioUID id = getUID(c);
  if(t){
//    PLAYERTYPE * p = new PLAYERTYPE();
    assert(players.count(id)==0);
    players[id] = std::make_shared<PLAYERTYPE>();;
//    players[id].
    if(auto cP = players[id]){
      if(!cP->load(c.getAudioPath(),true)){
        ofLogError("AudioPlayer")<<"can't load : " << c.getAudioPath() << " : " << id.name;}
    }
#ifdef USE_FMODPLAYER
    FMOD_Channel_SetCallback(players[id]->channel,&AudioPlayer::gotFmodEvent);
#endif
  }
  else{

    players[id]->stop();
    players.erase(id);
    playNeedles[id] = STOPNEEDLE;
  }
  //    players[id]->setMultiPlay(true);
}


void AudioPlayer::stopAll(){
  ofScopedLock sl(staticMutex);

  for(auto & p:players){

    if(auto cP = p.second)cP->stop();

    playNeedles[p.first]  =STOPNEEDLE;

  }
  players.clear();

}

bool AudioPlayer::Play(Container & c, float _s){
  audioUID id = getUID(c);

  // s : state
  // _s : volume
  int s = 0;
  if(_s > 0 ){
    s = 1;
  }


  {
    std::lock_guard<std::mutex> sl(staticMutex);
    map<audioUID,AudioPlayer::PLAYERPTR>::iterator it = players.find(id);

    // play existing and playing

    if(it!=players.end()){
      //restart
      if(s >0){
        if(auto cP = it->second){
        DEBUGPRINT_AUDIO("restart playing " << id.toString() << "  " <<ofGetElapsedTimef() );

        if(!cP->isPlaying())cP->play();
        cP->setPositionMS(c.begin*1000.0);
        
        cP->setVolume(globalVolume * _s);
        if(maxTime!=0){
          float maxTimec = MIN(c.end - c.begin,maxTime);
          playNeedles[id] = -c.begin + maxTimec;
        }
        else{
          playNeedles[id] =(c.end -c.begin);
        }
        ofLogNotice("Audio") << "playing for "<< playNeedles[id];

        DEBUGPRINT_AUDIO("end load playing " << ofGetElapsedTimef() );


      }
      }
      //stop it
      else if( s ==0){
        if(auto cP = it->second)cP->stop();
        playNeedles[it->first] = STOPNEEDLE;
      }
    }

    // load on the go

    else if(s==1){

      bool hasOnePreloaded =false;
      //try play already loaded
      vector<audioUID> toRemove;

      for(map<audioUID,AudioPlayer::PLAYERPTR>::iterator p= players.begin();p!=players.end();++p){

        // start preloaded
        if(s == 1 && p->first.name == id.name){

          if(auto cP = p->second){

          if( !cP->isPlaying()){
          players[id] = p->second;
          cP->play();
          cP->setPositionMS(c.begin*1000.0);
          cP->setVolume(globalVolume);
          //            p->second->setStopMS((c.end-c.begin)*1000.0);
          if(maxTime!=0){
            float maxTimec = MIN(c.end - c.begin,maxTime);
            playNeedles[id] = -c.begin + maxTimec;
          }
          else{
            playNeedles[id] =(c.end-c.begin);
          }

          ofLogNotice("Audio") << "playing preloaded for "<< playNeedles[id];
          //            instance()->update(a);

          toRemove.push_back(p->first);


          hasOnePreloaded = true;
          break;

          }
          }
        }
      }

      for(auto r:toRemove){
        players.erase(r);
      }

      if(!hasOnePreloaded){
        DEBUGPRINT_AUDIO("loadOntheGo " << ofGetElapsedTimef() )

        Load(c,true);
        if(auto cPLayer = players[id])
        {
          cPLayer->play();
        cPLayer->setPositionMS(c.begin*1000.0);
        cPLayer->setVolume(globalVolume*_s);
        if(maxTime>0){
          float maxTimec = MIN(c.end - c.begin,maxTime);
          playNeedles[id] = -c.begin + maxTimec;
        }
        else{
          playNeedles[id] =(c.end-c.begin);
        }
          DEBUGPRINT_AUDIO( "playing for "<<c.begin*1000.0 <<","<<playNeedles[id]);
        }


      }
    }
  }


  ofEventArgs dumb;
  AudioPlayer::i()->update(dumb);

  return true;
}


void AudioPlayer::UnloadAll() {

  for(map<audioUID,AudioPlayer::PLAYERPTR>::iterator it = players.begin() ; it!= players.end() ;++it){
    if(auto cP =it->second){
      cP->unload();

    }

  }

  players.clear();

}


audioUID AudioPlayer::getUID(Container const & c){
  audioUID id;
  id.idx= c.globalIdx;
  
  id.name = c.getFilename();
  return id;
}


audioUID AudioPlayer::audioUIDfromString(const string s){
  audioUID res;
  vector<string> ss = ofSplitString(s,"\n");
  res.name = ss[0];//ofJoinString(s[0], " ");
  res.idx = ofFromString<int>(ss[1]);
  return res;
}

