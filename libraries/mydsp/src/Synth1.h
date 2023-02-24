#ifndef SYNTH1_H_INCLUDED
#define SYNTH1_H_INCLUDED

#include "SineTable.h"
#include "Phasor.h"

class Synth1{
public:
  Synth1(int SR);
  
  void setFrequency(float f);
  void setNoteOn(int index);
  void setNoteOff(int index);
  void setGain(float g);
  float tick();
private:
  SineTable sineTable;
  Phasor phasor;
  float gain;
  int samplingRate;
  bool *midiNotes;
  float getFrequencyFromMidi(int d);

};

#endif  // SINE_H_INCLUDED