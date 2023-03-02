#ifndef faust_teensy_h_
#define faust_teensy_h_

#include "Arduino.h"
#include "AudioStream.h"
#include "Audio.h"

#include "Phasor.h"
#include "Echo.h"
#include "Synth1.h"

class MyDsp : public AudioStream
{
  public:
    MyDsp();
    ~MyDsp();
    
    virtual void update(void);
    void setFreq(float freq);
    void setGain(float g);
    void setNoteOn(int index);
    void setNoteOff(int index);
    
  private:
    Synth1 synth;
};

#endif
