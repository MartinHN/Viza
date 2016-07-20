//
//  AudioPlayer.cpp
//  ViZa
//
//  Created by martin hermant on 09/10/14.
//
//

#include "AudioPlayer.h"
#include "ofFmodSoundPlayer.cpp"

#define DEBUG_AUDIO

#ifdef DEBUG_AUDIO
#define DEBUGPRINT_AUDIO(x) ofLogVerbose("Audio") << x;
#else
#define DEBUGPRINT_AUDIO(x) ;
#endif


#define STOPNEEDLE -100000


std::map<audioUID,ofFmodSoundPlayer*>  AudioPlayer::players;
std::map<audioUID,float> AudioPlayer::playNeedles;
AudioPlayer * AudioPlayer::inst;
float AudioPlayer::globalVolume = 0.6;
float AudioPlayer::maxTime = 0;
ofMutex AudioPlayer::staticMutex;

AudioPlayer::AudioPlayer(){

  buffersize = 256;
  ofFmodSoundPlayer::initializeFmod();

}

void AudioPlayer::update(ofEventArgs & a){

  //    ofFmodSoundUpdate();
  std::lock_guard<std::mutex> sl(staticMutex);
  vector<audioUID > toRemove;
  for(auto & n:playNeedles){
    if(players.count(n.first)){cout  <<","<< players[n.first]->getPositionMS() <<","<< n.second*1000.0  << endl;}

    if(players.count(n.first)>0){
      int isPlaying = 1;
      FMOD_Channel_IsPlaying(players[n.first]->channel,&isPlaying);
      if(isPlaying==0 ||
         (players[n.first]->getPositionMS() > n.second*1000.0 ) ||
         n.second==STOPNEEDLE
         ){
          ofLogNotice("Audio") << "stopping : " << n.first.toString();

          players[n.first]->stop();
          Container::containers[n.first.idx]->state = 0;
          delete players[n.first];
          players.erase(n.first);
          n.second = STOPNEEDLE;
          toRemove.push_back(n.first);

        }
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
  {

    vector<map<audioUID,ofFmodSoundPlayer*>::iterator> playerToRemove;
    for (map<audioUID,ofFmodSoundPlayer*>::iterator p = players.begin() ; p!=players.end() ;++p){
      if(p->second==NULL)playerToRemove.push_back(p);
    }
    for (auto & p:playerToRemove){
      players.erase(p);
    }
  }
}

FMOD_RESULT AudioPlayer::gotFmodEvent(FMOD_CHANNEL *channel, FMOD_CHANNEL_CALLBACKTYPE type, void *commanddata1, void *commanddata2){
  lock_guard<mutex> lk(staticMutex);
  for (auto & p:players){
    if(p.second->channel == channel && type==FMOD_CHANNEL_CALLBACKTYPE_END){
      playNeedles[p.first] = STOPNEEDLE;
      Container::containers[p.first.idx]->setState(0);
    }
  }
}

void AudioPlayer::Load(Container const & c,bool t){
  audioUID id = getUID(c);
  if(t){
    players[id] = new ofFmodSoundPlayer();

    if(!players[id]->load(c.getAudioPath(),true)){ofLogError("can't load : "+id.name);}
    FMOD_Channel_SetCallback(players[id]->channel,&AudioPlayer::gotFmodEvent);
  }
  else{

    players[id]->stop();
    delete players[id];
    players.erase(id);
    playNeedles[id] = STOPNEEDLE;
  }
  //    players[id]->setMultiPlay(true);
}


void AudioPlayer::stopAll(){
  ofScopedLock sl(staticMutex);

  for(auto & p:players){

    p.second->stop();
    delete p.second;
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
    map<audioUID,ofFmodSoundPlayer*>::iterator it = players.find(id);

    // play existing and playing

    if(it!=players.end()){
      //restart
      if(s >0 && it->second!=nullptr){
        DEBUGPRINT_AUDIO("restart playing " << id.toString() << "  " <<ofGetElapsedTimef() );

        it->second->play();
        it->second->setPositionMS(c.begin*1000.0);
        it->second->setVolume(globalVolume * _s);
        if(maxTime!=0){
          float maxTimec = MIN(c.end - c.begin,maxTime);
          playNeedles[id] = c.begin + maxTimec;
        }
        else{
          playNeedles[id] =(c.end);
        }
        ofLogNotice("Audio") << "playing for "<< playNeedles[id];

        DEBUGPRINT_AUDIO("end load playing " << ofGetElapsedTimef() );


      }
      //stop it
      else if( s ==0){
        if(it->second)it->second->stop();
        playNeedles[it->first] = STOPNEEDLE;
      }
    }

    // load on the go

    else if(s==1){

      bool hasOnePreloaded =false;
      //try play already loaded
      vector<audioUID> toRemove;

      for(map<audioUID,ofFmodSoundPlayer*>::iterator p= players.begin();p!=players.end();++p){

        // start preloaded
        if(s == 1 && p->first.name == id.name && p->second!=NULL && !p->second->isPlaying()){
          players[id] = p->second;
          p->second->play();
          p->second->setPositionMS(c.begin*1000.0);
          p->second->setVolume(globalVolume);
          //            p->second->setStopMS((c.end-c.begin)*1000.0);
          if(maxTime!=0){
            float maxTimec = MIN(c.end - c.begin,maxTime);
            playNeedles[id] = c.begin + maxTimec;
          }
          else{
            playNeedles[id] =(c.end);
          }

          ofLogNotice("Audio") << "playing for "<< playNeedles[id];
          //            instance()->update(a);

          toRemove.push_back(p->first);


          hasOnePreloaded = true;
          break;


        }
      }

      for(auto r:toRemove){
        players.erase(r);
      }

      if(!hasOnePreloaded){
        DEBUGPRINT_AUDIO("loadOntheGo " << ofGetElapsedTimef() )

        Load(c,true);
        players[id]->play();
        players[id]->setPositionMS(c.begin*1000.0);
        players[id]->setVolume(globalVolume*_s);
        if(maxTime>0){
          float maxTimec = MIN(c.end - c.begin,maxTime);
          playNeedles[id] = c.begin + maxTimec;
        }
        else{
          playNeedles[id] =(c.end);
        }

        ofLogNotice("Audio") << "playing for "<< playNeedles[id];
      }
    }
  }


  ofEventArgs dumb;
  AudioPlayer::i()->update(dumb);

  return true;
}


void AudioPlayer::UnloadAll() {

  for(map<audioUID,ofFmodSoundPlayer*>::iterator it = players.begin() ; it!= players.end() ;++it){
    if(it->second!=NULL){
      it->second->unload();
      delete it->second;
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

